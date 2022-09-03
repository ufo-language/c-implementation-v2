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
static struct D_HashTable* _blocked;
static int _nBlocked;
static int _nSteps = 10;

void threadManager_runNextThread(void);

void threadManager_addThread(struct Evaluator* etor) {
    queue_enq(_running, (struct Any*)etor);
    _nRunning++;
}

void threadManager_runAll(void) {
    while (_nRunning > 0) {
        threadManager_runNextThread();
    }
}

void threadManager_runNextThread(void) {
    struct Evaluator* etor = (struct Evaluator*)queue_deq_unsafe(_running);
    evaluator_runSteps(etor, _nSteps);
    enum ThreadStatus status = evaluator_getThreadStatus(etor);
    switch (status) {
        case TS_Running:
            queue_enq(_running, (struct Any*)etor);
            break;
        case TS_Blocked: {
                struct Any* blockingObject = evaluator_getBlockingObject(etor);
                hashTable_put_unsafe(_blocked, blockingObject, (struct Any*)etor);
            }
            _nRunning--;
            _nBlocked++;
            break;
        case TS_Terminated:
            _nRunning--;
            break;
    }
}

void threadManager_rootObjects(void) {
    _running = queue_new();
    _blocked = hashTable_new();
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
