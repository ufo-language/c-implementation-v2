#include <stdlib.h>
#include <time.h>

#include "gc/gc.h"
#include "main/ufo.h"

int ARGC = 0;
char** ARGV = NULL;

void any_sanityCheck(void);
void methodTable_setupMethods(void);
void methodTable_deallocateAll(void);

int main(int argc, char* argv[]) {
#if defined(linux)
    char* name = "ufo_evaluator";
    if (prctl(PR_SET_NAME, name, NULL, NULL, NULL) < 0) {
        perror("prctl()");
    }
#endif
    ARGC = argc;
    ARGV = argv;
    srand(time(0));
    any_sanityCheck();
    methodTable_setupMethods();
    gc_start();
    ufo_start();  // this starts the REPL
    ufo_stop();
    gc_stop();
    methodTable_deallocateAll();
    return 0;
}
