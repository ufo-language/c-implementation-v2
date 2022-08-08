#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/file.h"
#include "data/list.h"
#include "data/string.h"
#include "data/stringbuffer.h"
#include "etor/evaluator.h"
#include "gc/gc.h"
#include "lexer/lexobj.h"
#include "main/globals.h"
#include "parser/parser.h"
#include "repl/coloncommand.h"
#include "repl/repl.h"
#include "version.h"

struct REPL* repl_new(bool makeRoot) {
    struct REPL* self = (struct REPL*)gc_alloc(T_REPL);
    if (makeRoot) {
        gc_transferToRoots((struct Any*)self);
    }
    self->inputStringBuffer = stringBuffer_new();
    self->inputString = EMPTY_STRING;
    self->string = EMPTY_STRING;
    self->tokens = EMPTY_LIST;
    self->colonTokens = EMPTY_LIST;
    self->expr = (struct Any*)NIL;
    self->value = (struct Any*)NIL;
    self->etor = (struct Evaluator*)evaluator_new();
    self->error = (struct Any*)NIL;
    self->fileWasLoaded = false;
    self->loadFileName = EMPTY_STRING;
    self->keepRunning = true;
    return self;
}

void repl_free(struct REPL* self) {
    free(self);
}

struct D_String* repl_getInputString(struct REPL* self) {
    return self->string;
}

void repl_markChildren(struct REPL* self) {
    any_mark((struct Any*)self->inputStringBuffer);
    any_mark((struct Any*)self->inputString);
    any_mark((struct Any*)self->string);
    any_mark((struct Any*)self->tokens);
    any_mark((struct Any*)self->colonTokens);
    any_mark((struct Any*)self->expr);
    any_mark((struct Any*)self->value);
    any_mark((struct Any*)self->etor);
    any_mark((struct Any*)self->error);
    any_mark((struct Any*)self->loadFileName);
}

static void repl_intro(void) {
    puts("▌▐ ▛▘▛▜ UFO version " UFO_VERSION);
    puts("▙▟ ▛ ▙▟ http://github.com/ufo-language");
    puts("type :? for help");
    puts("");
}

static void repl_prompt(void) {
    printf("UFO> ");
}

extern volatile sig_atomic_t SIGNAL_STATUS;

static int repl_readFile(struct REPL* self) {
    char* fileName = string_getChars(self->loadFileName);
    struct D_File* file = file_new_charString(fileName);
    if (file_open_aux(file)) {
        file_readAll_stringBuffer(file, self->inputStringBuffer, self->etor);
        file_close(file, self->etor);
        return stringBuffer_count(self->inputStringBuffer);
    }
    return -1;
}

// Returns the number of characters read, or -1 on EOF (^D)
static int repl_readLine(struct D_StringBuffer* line) {
    while (true) {
        int c = getchar();
        if (c == '\n') {
            break;
        }
        if (c == -1) {  // -1 is EOF
            if (SIGNAL_STATUS > 0) {
                // if the user pressed ^C, then execution reaches this point
                SIGNAL_STATUS = 0;
                // the terminal has likely displayed "^C" without a newline, so print a newline
                putchar('\n');
                return 0;
            }
            return -1;
        }
        stringBuffer_writeChar(line, c);
    }
    return stringBuffer_count(line);
}

static int repl_readLines(struct REPL* self) {
    stringBuffer_clear(self->inputStringBuffer);
    struct D_StringBuffer* line = stringBuffer_new();
    int nCharsLines = 0;
    while (true) {
        int nCharsLine = repl_readLine(line);
        if (nCharsLine == -1) {
            return -1;
        }
        if (nCharsLine == 0) {
            break;
        }
        struct D_String* lineString = stringBuffer_asString(line);
        stringBuffer_write(self->inputStringBuffer, lineString);
        stringBuffer_writeChar(self->inputStringBuffer, '\n');
        nCharsLines += nCharsLine + 1;
        stringBuffer_clear(line);
    }
    return nCharsLines;
}

// Reads a string from stdin or from a file
static int repl_read(struct REPL* self) {
    int nChars;
    repl_prompt();
    stringBuffer_clear(self->inputStringBuffer);
    nChars = repl_readLine(self->inputStringBuffer);
    if (nChars > 0) {
        self->inputString = stringBuffer_asString(self->inputStringBuffer);
        char* chars = string_getChars(self->inputString);
        if (':' == chars[0]) {
            self->colonTokens = string_split(self->inputString, ' ');
            enum ReadAction readAction = colonCommand_run(self->colonTokens, self);
            switch (readAction) {
                case KEEP_LOOPING:
                    break;
                case QUIT:
                    self->keepRunning = false;
                    break;
                case READ_LINES:
                    nChars = repl_readLines(self);
                    break;
                case READ_FILE:
                    nChars = repl_readFile(self);
                    break;
            }
        }
    }
    return nChars;
}

static bool repl_tokenize(struct REPL* self) {
    struct D_List* tokens = lexObj_string(self->string);
    if (tokens == NULL) {
        self->tokens = EMPTY_LIST;
        return false;
    }
    self->tokens = tokens;
    return true;
}

static bool repl_parse(struct REPL* self) {
    struct Any* expr = parser_parse(&self->tokens, self->etor);
    if (expr == NULL) {
        self->expr = (struct Any*)NIL;
        return false;
    }
    self->expr = expr;
    return true;
}

// Evaluates the object.
static bool repl_eval(struct REPL* self) {
    evaluator_pushExpr(self->etor, self->expr);
    evaluator_run(self->etor);
    struct Any* error = evaluator_getException(self->etor);
    if (error != (struct Any*)NIL) {
        self->error = error;
        return false;
    }
    return true;
}

// Displays the result.
static void repl_printError(struct REPL* self) {
    fputs("Evaluation error: ", stderr);
    any_show(self->error, stderr);
    fputc('\n', stderr);
    evaluator_clearException(self->etor);
    self->error = (struct Any*)NIL;
}

// Displays the result.
static bool repl_printValue(struct REPL* self) {
    struct Any* value = evaluator_popObj(self->etor);
    self->value = value;
    if (value != (struct Any*)NIL) {
        any_show(self->value, stdout);
        printf(" :: %s\n", any_typeName(self->value));
    }
    return true;
}

// This is the rewrite of the repl_run function below.
void repl_run(struct REPL* self) {
    repl_intro();
    while (true) {
        int nChars = repl_read(self);
        if (!self->keepRunning) {
            break;
        }
        bool res =
            repl_tokenize(self)
            && repl_parse(self)
            && repl_eval(self);
        if (res) {
            repl_printError(self);
        }
        else {
            repl_printValue(self);
        }
        gc_commit();
    }
    fputc('\n', stdout);
}

#if 0
void repl_run_X(struct REPL* self) {
    repl_intro();
    bool keepLooping = true;
    while (keepLooping) {
        int nChars;
        if (!self->fileWasLoaded) {
            stringBuffer_clear(self->inputStringBuffer);
            repl_prompt();
            int res = repl_readLine(self->inputStringBuffer);
            if (res == -1) {
                break;
            }
        }
        self->fileWasLoaded = false;
        nChars = stringBuffer_count(self->inputStringBuffer);
        if (nChars > 0) {
            self->inputString = stringBuffer_asString(self->inputStringBuffer);
            stringBuffer_clear(self->inputStringBuffer);
            char* chars = string_getChars(self->inputString);
            if (':' == chars[0]) {
                // TODO I'm pretty sure these tokens don't need to be attached to the REPL.
                // I created the colonTokens field in order to diagnose GC bugs.
                self->colonTokens = string_split(self->inputString, ' ');
                keepLooping = colonCommand_run(self->colonTokens, self);
            }
            else {
                // tokenize the string
                self->string = self->inputString;
                struct D_List* tokens = lexObj_string(self->string);
                if (tokens != NULL) {
                    // parse the tokens
                    self->tokens = tokens;
                    while (true) {
                        struct D_Array* firstToken = (struct D_Array*)list_getFirst(tokens);
                        if (array_get_unsafe(firstToken, 0) == (struct Any*)LEXER_SYMBOLS[LT_EOI]) {
                            break;
                        }
                        struct Any* expr = parser_parse(&tokens, self->etor);
                        if (expr == NULL) {
                            break;
                        }
                        // evaluate the expression
                        self->expr = expr;
                        evaluator_pushExpr(self->etor, self->expr);
                        evaluator_run(self->etor);
                        struct Any* error = evaluator_getException(self->etor);
                        if (error != (struct Any*)NIL) {
                            self->error = error;
                            fputs("Evaluation error: ", stderr);
                            any_show(error, stderr);
                            fputc('\n', stderr);
                            evaluator_clearException(self->etor);
                        }
                        else {
                            self->error = (struct Any*)NIL;
                            struct Any* value = evaluator_popObj(self->etor);
                            self->value = value;
                            if (value != (struct Any*)NIL) {
                                any_show(self->value, stdout);
                                printf(" :: %s\n", any_typeName(self->value));
                            }
                        }
                    }
                }
            }
        }
        gc_commit();
    }
    fputc('\n', stdout);
}
#endif

void repl_show(struct REPL* self, FILE* fp) {
    (void)self;
    fprintf(fp, "REPL{inputStringBuffer="); any_show((struct Any*)self->inputStringBuffer, fp);
    fprintf(fp, ", inputString="); any_show((struct Any*)self->inputString, fp);
    fprintf(fp, ", string="); any_show((struct Any*)self->string, fp);
    fprintf(fp, ", tokens="); any_show((struct Any*)self->tokens, fp);
    fprintf(fp, ", colonTokens="); any_show((struct Any*)self->colonTokens, fp);
    fprintf(fp, ", expr="); any_show((struct Any*)self->expr, fp);
    fprintf(fp, ", value="); any_show((struct Any*)self->value, fp);
    fprintf(fp, ", etor="); any_show((struct Any*)self->etor, fp);
    fprintf(fp, ", error="); any_show((struct Any*)self->error, fp);
    fprintf(fp, ", fileName="); any_show((struct Any*)self->loadFileName, fp);
    fputc('}', fp);
}

size_t repl_sizeOf(struct REPL* self) {
    return sizeof(self);
}

size_t repl_structSize() {
    return sizeof(struct REPL);
}
