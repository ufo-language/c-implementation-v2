#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data/any.h"
#include "data/array.h"
#include "data/file.h"
#include "data/list.h"
#include "data/string.h"
#include "data/stringbuffer.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "gc/gc.h"
#include "lexer/lexer.h"
#include "main/globals.h"
#include "methods/methods.h"
#include "parser/parser.h"
#include "repl/coloncommand.h"
#include "repl/repl.h"
#include "version.h"

struct Methods* repl_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_free = (void (*)(struct Any*))repl_free;
    methods->m_markChildren = (void (*)(struct Any* self))repl_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))repl_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))repl_sizeOf;
    methods->m_structSize = repl_structSize;
    return methods;
}

struct REPL* repl_new(bool makeRoot) {
    struct REPL* self = (struct REPL*)gc_alloc(T_REPL);
    if (makeRoot) {
        gc_transferToRoots((struct Any*)self);
    }
    self->inputStringBuffer = stringBuffer_new();
    self->inputString = EMPTY_STRING;
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

// If the only token in the token list is an EOI token,
// then remove it.
void repl_consumeEOIToken(struct REPL* self) {
    if (list_isEmpty(self->tokens)) {
        return;
    }
    struct D_Array* token = (struct D_Array*)list_getFirst(self->tokens);
    struct D_Symbol* tokenSym = (struct D_Symbol*)array_get_unsafe(token, 0);
    if (strcmp("EOI", symbol_getName(tokenSym))) {
        return;
    }
    self->tokens = EMPTY_LIST;
}

struct D_String* repl_getInputString(struct REPL* self) {
    return self->inputString;
}

void repl_markChildren(struct REPL* self) {
    any_mark((struct Any*)self->inputStringBuffer);
    any_mark((struct Any*)self->inputString);
    any_mark((struct Any*)self->inputString);
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
    struct D_File* file = file_new_charStringName(fileName);
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

// Reads a string from stdin or from a file.
// Places the string in self->inputString.
// Returns the number of characters read.
static int repl_read(struct REPL* self) {
    stringBuffer_clear(self->inputStringBuffer);
    int nChars = repl_readLine(self->inputStringBuffer);
    if (nChars > 0) {
        self->inputString = stringBuffer_asString(self->inputStringBuffer);
        char* chars = string_getChars(self->inputString);
        if (':' == chars[0]) {
            self->colonTokens = string_split(self->inputString, ' ');
            enum ReadAction readAction = colonCommand_run(self->colonTokens, self);
            switch (readAction) {
                case KEEP_LOOPING:
                    nChars = 0;
                    break;
                case QUIT:
                    self->keepRunning = false;
                    break;
                case READ_LINES:
                    stringBuffer_clear(self->inputStringBuffer);
                    nChars = repl_readLines(self);
                    break;
                case READ_FILE:
                    stringBuffer_clear(self->inputStringBuffer);
                    nChars = repl_readFile(self);
                    if (nChars == -1) {
                        fprintf(stderr, "File not found: ");
                        any_show((struct Any*)self->loadFileName, stderr);
                        fputc('\n', stderr);
                        nChars = 0;
                    }
                    break;
            }
            self->inputString = stringBuffer_asString(self->inputStringBuffer);
        }
    }
    return nChars;
}

static bool repl_tokenize(struct REPL* self) {
    struct Any* res = lexer_tokenize(self->inputString);
    if (res->typeId == T_Array) {
        fprintf(stderr, "Exception: ");
        any_show(res, stderr);
        fprintf(stderr, "\n");
        return false;
    }
    self->tokens = (struct D_List*)res;
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

static void repl_printError(struct REPL* self) {
    fputs("Evaluation error: ", stderr);
    any_show(self->error, stderr);
    fputc('\n', stderr);
    evaluator_clearException(self->etor);
    self->error = (struct Any*)NIL;
}

static bool repl_printValue(struct REPL* self) {
    self->value = evaluator_popObj(self->etor);
    if (self->value != (struct Any*)NIL) {
        any_show(self->value, stdout);
        printf(" :: %s\n", any_typeName(self->value));
    }
    return true;
}

void repl_run(struct REPL* self) {
    repl_intro();
    while (true) {
        if (list_isEmpty(self->tokens)) {
            repl_prompt();
            int nChars = repl_read(self);
            if (!self->keepRunning) {
                break;
            }
            if (nChars == -1) {
                // user pressed ^D
                break;
            }
            if (nChars == 0) {
                // user pressed Enter without typing anything
                continue;
            }
            if (!repl_tokenize(self)) {
                // the repl_tokenize function displays the error message
                continue;
            }
        }
        if (!repl_parse(self)) {
            // the parser displays the error message
            continue;
        }
        if (!repl_eval(self)) {
            repl_printError(self);
        }
        else {
            repl_printValue(self);
        }
        repl_consumeEOIToken(self);
        gc_commit();
    }
    fputc('\n', stdout);
}

void repl_show(struct REPL* self, FILE* fp) {
    (void)self;
    fprintf(fp, "REPL{inputStringBuffer="); any_show((struct Any*)self->inputStringBuffer, fp);
    fprintf(fp, ", inputString="); any_show((struct Any*)self->inputString, fp);
    fprintf(fp, ", string="); any_show((struct Any*)self->inputString, fp);
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

size_t repl_structSize(void) {
    return sizeof(struct REPL);
}
