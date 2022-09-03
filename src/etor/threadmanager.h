#if !defined(THREAD_MANAGER)
#define THREAD_MANAGER

struct D_Symbol;

void threadManager_addThread(struct Evaluator* etor);
void threadManager_rootObjects(void);
void threadManager_runAll(void);
struct D_Symbol* threadManager_statusSymbol(enum ThreadStatus status);

#endif
