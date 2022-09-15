#include "data/any.h"
#include "data/hashtable.h"
#include "data/queue.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "etor/threadmanager.h"
#include "main/globals.h"

struct D_Symbol* SYM_DORMANT;
struct D_Symbol* SYM_RUNNING;
struct D_Symbol* SYM_BLOCKED;
struct D_Symbol* SYM_TERMINATED;

static struct D_Queue* _running;
static int _nSteps = 10;

void threadManager_runNextThread(void);

void threadManager_addThread(struct Evaluator* thread) {
    if (TS_Running != evaluator_getThreadStatus(thread)) {
        evaluator_setThreadStatus(thread, TS_Running);
        queue_enq(_running, (struct Any*)thread);
    }
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
}

int threadManager_count(void) {
    return queue_count(_running);
}

void threadManager_runAll(void) {
    while (queue_count(_running) > 0) {
        threadManager_runNextThread();
    }
    //printf("%s finished running all threads\n", __func__);
}

void threadManager_rootObjects(void) {
    _running = queue_new();
    SYM_DORMANT = symbol_new("Dormant");
    SYM_RUNNING = symbol_new("Running");
    SYM_BLOCKED = symbol_new("Blocked");
    SYM_TERMINATED = symbol_new("Terminated");
}

void threadManager_runNextThread(void) {
    // leave the thread attached to the queue while it's running
    struct Evaluator* thread = (struct Evaluator*)queue_peek_unsafe(_running);
    evaluator_runSteps(thread, _nSteps);
    (void)queue_deq_unsafe(_running);
    if (TS_Running == evaluator_getThreadStatus(thread)) {
        queue_enq(_running, (struct Any*)thread);
    }
}

struct D_Symbol* threadManager_statusSymbol(enum ThreadStatus status) {
    struct D_Symbol* sym = NULL;
    switch (status) {
        case TS_Dormant:
            sym = SYM_DORMANT;
            break;
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
}

void threadManager_unblockThread(struct Evaluator* thread) {
    evaluator_setBlockingObject(thread, (struct Any*)NIL);
    threadManager_addThread(thread);
}
