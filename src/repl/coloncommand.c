#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "data/any.h"
#include "data/file.h"
#include "data/list.h"
#include "data/string.h"
#include "data/stringbuffer.h"
#include "etor/evaluator.h"
#include "main/globals.h"
#include "memory/gc.h"
#include "repl/coloncommand.h"
#include "repl/repl.h"
#include "utils/utils.h"

static void colonCommand_help(void) {
    puts(" UFO");
    puts("  :?              Shows this help message");
    puts("  :q              Quits UFO");
    puts("  :l              Reloads last file");
    puts("  :l <file>       Loads source file");
    puts(" REPL");
    puts("  :e              Enter multiple lines, empty line to terminate");
    puts("  :env            Shows current dynamic environment");
    puts("  :err            Shows last evaluation error");
    puts("  :expr           Shows last parse result");
    puts("  :i              Shows last input string");
    puts("  :r              Shows all REPL fields");
    puts("  :tok            Shows last list of lexer tokens");
    puts("  :val            Shows last evaluation result");
    puts(" Evaluator");
    puts("  :eval show      Shows the current evaluator");
    puts("  :eval trace     Shows evaluation steps");
    puts("  :eval notrace   Hides evaluation steps");
    puts(" Garbage collector");
    puts("  :gc             Performs a garbage collection");
    puts("  :gc disable     Disables automatic garbage collection");
    puts("  :gc nobjs <n>   Trigger GC on <n> objects");
    puts("  :gc heap <size> Trigger GC on <size> bytes");
    puts("  :gcd            Dumps garbage collector roots and spine");
    puts("  :gcm            Performs a GC mark phase, followed by a dump");
    puts("  :gcs            Shows garbage collector statistics");
    puts(" Other");
    puts("  :idents         Shows identifier intern table");
    puts("  :syms           Shows symbol intern table");
}

enum ReadAction colonCommand_run(struct D_List* tokens, struct REPL* repl) {
    struct D_String* token1 = (struct D_String*)list_getFirst(tokens);
    char* string = string_getChars(token1);
    repl->fileWasLoaded = false;
    // UFO -----------------------------------------------------------
    if (!strcmp(":?", string)) {
        colonCommand_help();
    }
    else if (!strcmp(":q", string)) {
        return QUIT;
    }
    else if (!strcmp(":l", string)) {
        if (list_isEmpty((struct D_List*)list_getRest(tokens))) {
            if (repl->loadFileName == NULL) {
                puts("no file was previously loaded");
                return KEEP_LOOPING;
            }
        }
        else {
            repl->loadFileName = (struct D_String*)list_getSecond(tokens);
        }
        return READ_FILE;
    }
    // REPL ----------------------------------------------------------
    else if (!strcmp(":e", string)) {
        return READ_LINES;
    }
    else if (!strcmp(":env", string)) {
        struct D_Triple* env = evaluator_getEnv(repl->etor);
        any_show((struct Any*)env, stdout);
        printf("\n");
    }
    else if (!strcmp(":err", string)) {
        printf("Option not implemented\n");
    }
    else if (!strcmp(":expr", string)) {
        any_show((struct Any*)repl->expr, stdout);
        printf(" :: %s\n", any_typeName(repl->expr));
    }
    else if (!strcmp(":i", string)) {
        any_show((struct Any*)repl_getInputString(repl), stdout);
        printf("\n");
    }
    else if (!strcmp(":r", string)) {
        repl_show(repl, stdout);
        putchar('\n');
    }
    else if (!strcmp(":tok", string)) {
        any_show((struct Any*)repl->tokens, stdout);
        printf("\n");
    }
    else if (!strcmp(":val", string)) {
        any_show((struct Any*)repl->value, stdout);
        printf(" :: %s\n", any_typeName(repl->value));
    }
    // Evaluator -----------------------------------------------------
    else if (!strcmp(":eval", string)) {
        if (!list_isEmpty((struct D_List*)list_getRest(tokens))) {
            struct D_String* token2 = (struct D_String*)list_getSecond(tokens);
            char* string2 = string_getChars(token2);
            if (!strcmp("trace", string2)) {
                evaluator_setShowSteps(repl->etor, true);
            }
            else if (!strcmp("notrace", string2)) {
                evaluator_setShowSteps(repl->etor, false);
            }
            else if (!strcmp("show", string2)) {
                any_show((struct Any*)repl->etor, stdout);
                putchar('\n');
            }
            else {
                printf("Argument not valid '%s'\n", string2);
            }
        }
        else {
            printf("please supply an argument\n");
        }
    }
    else if (!strcmp(":trap", string)) {
        printf("Option not implemented\n");
    }
    // Garbage collector ---------------------------------------------
    else if (!strcmp(":gc", string)) {
        printf("Collecting garbage\n");
        gc_collect();
    }
    else if (!strcmp(":gcd", string)) {
        printf("GC dump\n");
        gc_dump();
    }
    else if (!strcmp(":gcm", string)) {
        printf("GC mark\n");
        gc_markRoots();
        gc_dump();
    }
    else if (!strcmp(":gcs", string)) {
        printf("GC stats\n");
        gc_showStats();
    }
    // Other collector -----------------------------------------------
    else if (!strcmp(":idents", string)) {
        any_show((struct Any*)INTERNED_IDENTIFIERS, stdout);
        putchar('\n');
    }
    else if (!strcmp(":syms", string)) {
        any_show((struct Any*)INTERNED_SYMBOLS, stdout);
        putchar('\n');
    }
    else {
        fprintf(stderr, "Colon command '%s' not understood\n", string);
    }
    return KEEP_LOOPING;
}
