#include "data/any.h"
#include "data/hashtable.h"
#include "data/queue.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "etor/threadmanager.h"

struct D_Symbol* SYM_RUNNING;
struct D_Symbol* SYM_BLOCKED;
struct D_Symbol* SYM_TERMINATED;

static struct D_Queue* _running;
static int _nRunning;
static int _nBlocked;
static int _nSteps = 10;

void threadManager_runNextThread(void);

void threadManager_addThread(struct Evaluator* thread) {
    queue_enq(_running, (struct Any*)thread);
    _nRunning++;
}

void threadManager_blockThread(struct Evaluator* thread, struct Any* blockingObject) {
    if (T_Evaluator == any_typeId(blockingObject)) {
        struct Evaluator* blockingThread = (struct Evaluator*)blockingObject;
        if (TS_Terminated == evaluator_getThreadStatus(blockingThread)) {
            // blocking thread is already terminated
            return;
        }
        evaluator_addWaitingThread((struct Evaluator*)blockingObject, thread);
    }
    evaluator_setThreadStatus(thread, TS_Blocked);
    evaluator_setBlockingObject(thread, blockingObject);
    _nRunning--;
    _nBlocked++;
}

void threadManager_unblockThread(struct Evaluator* thread) {
    evaluator_setThreadStatus(thread, TS_Running);
    threadManager_addThread(thread);
    _nBlocked--;
}

void threadManager_runAll(void) {
    while (_nRunning > 0) {
        threadManager_runNextThread();
    }
}

void threadManager_runNextThread(void) {
    struct Evaluator* thread = (struct Evaluator*)queue_deq_unsafe(_running);
    evaluator_runSteps(thread, _nSteps);
    if (TS_Running == evaluator_getThreadStatus(thread)) {
        queue_enq(_running, (struct Any*)thread);
    }
}

void threadManager_rootObjects(void) {
    _running = queue_new();
    SYM_RUNNING = symbol_new("Running");
    SYM_BLOCKED = symbol_new("Blocked");
    SYM_TERMINATED = symbol_new("Terminated");
}

struct D_Symbol* threadManager_statusSymbol(enum ThreadStatus status) {
    struct D_Symbol* sym = NULL;
    switch (status) {
        case TS_Running:
            sym = SYM_RUNNING;
            break;
        case TS_Blocked:
            sym = SYM_BLOCKED;
            break;
        case TS_Terminated:
            sym = SYM_TERMINATED;
            break;
    }
    return sym;
}

void threadManager_terminateThread(struct Evaluator* thread) {
    evaluator_setThreadStatus(thread, TS_Terminated);
    evaluator_unblockWaitingThreads(thread);
    _nRunning--;
}
