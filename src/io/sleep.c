#include <sys/time.h>
#include <time.h>

#include "data/any.h"
#include "data/real.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "main/globals.h"
#include "utils/time.h"

static void _sleep_contin(struct E_Continuation* contin, struct Evaluator* etor) {
    struct D_Real* timeoutObj = (struct D_Real*)continuation_getArg(contin);
    double timeout = real_getValue(timeoutObj);
    if (timeNow_double() < timeout) {
        evaluator_setThreadStatus(etor, TS_Yield);
        evaluator_pushExpr(etor, (struct Any*)contin);
    }
    else {
        evaluator_pushObj(etor, (struct Any*)NIL);
    }
}

void io_sleep_nonBlocking(struct Evaluator* etor, struct D_Real* delay) {
    double timeout = timeNow_double() + real_getValue(delay);
    struct D_Real* timeoutObj = real_new(timeout);
    struct E_Continuation * contin = continuation_new(_sleep_contin, "sleep", (struct Any*)timeoutObj);
    _sleep_contin(contin, etor);
}
