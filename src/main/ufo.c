#include <signal.h>

#include "data/any.h"
#include "gc/gc.h"
#include "main/ufo.h"
#include "repl/repl.h"

#define CAPTURE_SIGINT 0

volatile sig_atomic_t SIGNAL_STATUS = 0;

void sigIntHandler(int signalValue) {
    SIGNAL_STATUS = signalValue;
    if (signal(SIGINT, sigIntHandler) == SIG_ERR) {
        fprintf(stderr, "WARNING: %s failed to set SIGINT handler\n", __func__);
    }
}

void ufo_start(void) {
#if CAPTURE_SIGINT
    if (signal(SIGINT, sigIntHandler) == SIG_ERR) {
        fprintf(stderr, "WARNING: %s failed to set SIGINT handler\n", __func__);
    }
#endif
    // this flag will eventually be set based on what command line options are used
    bool isInteractive = true;
    if (isInteractive) {
        struct REPL* repl = repl_new(true);
        repl_run(repl);
    }
    else {
        fprintf(stderr, "ufo_start: not an interactive session, now what?\n");
    }
}

void bucketLink_freeAll(void);

void ufo_stop(void) {
    bucketLink_freeAll();
}
