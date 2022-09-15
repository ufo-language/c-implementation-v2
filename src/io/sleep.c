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

static void _sleep_contin(struct Evaluator* etor, struct Any* arg) {
    struct Any* res = (struct Any*)NIL;
    struct D_Real* timeoutObj = (struct D_Real*)arg;
    double timeout = real_getValue(timeoutObj);
    if (_timeNow() < timeout) {
        res = (struct Any*)continuation_new(_sleep_contin, "sleep", arg);
    }
    evaluator_pushExpr(etor, res);
}

void io_sleep_nonBlocking(struct Evaluator* etor, struct D_Real* delay) {
    double timeout = _timeNow() + real_getValue(delay);
    struct D_Real* timeoutObj = real_new(timeout);
    _sleep_contin(etor, (struct Any*)timeoutObj);
}
