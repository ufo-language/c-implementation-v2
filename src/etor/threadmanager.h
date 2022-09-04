#if !defined(THREAD_MANAGER)
#define THREAD_MANAGER

struct D_Symbol;

void threadManager_addThread(struct Evaluator* thread);
void threadManager_blockThread(struct Evaluator* thread, struct Any* blockingObject);
void threadManager_rootObjects(void);
void threadManager_runAll(void);
struct D_Symbol* threadManager_statusSymbol(enum ThreadStatus status);
void threadManager_terminateThread(struct Evaluator* thread);

#endif
