#include <sys/time.h>
#include <time.h>

#include "data/any.h"
#include "data/real.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "main/globals.h"

static double _timeNow() {
    struct timeval te;
    gettimeofday(&te, NULL);
    double timeNow = te.tv_sec + te.tv_usec / 1E6;
    return timeNow;
 }

static void _sleep_contin(struct E_Continuation* contin, struct Evaluator* etor) {
    struct D_Real* timeoutObj = (struct D_Real*)continuation_getArg(contin);
    double timeout = real_getValue(timeoutObj);
    struct Any* res = _timeNow() < timeout ? (struct Any*)contin : (struct Any*)NIL;
    evaluator_pushExpr(etor, res);
}

void io_sleep_nonBlocking(struct Evaluator* etor, struct D_Real* delay) {
    double timeout = _timeNow() + real_getValue(delay);
    struct D_Real* timeoutObj = real_new(timeout);
    struct E_Continuation * contin = continuation_new(_sleep_contin, "sleep", (struct Any*)timeoutObj);
    _sleep_contin(contin, etor);
}
