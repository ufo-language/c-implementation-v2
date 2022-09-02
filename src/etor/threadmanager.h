#if !defined(THREAD_MANAGER)
#define THREAD_MANAGER

void threadManager_addThread(struct Evaluator* etor);
void threadManager_rootObjects(void);
void threadManager_runAll(void);

#endif
