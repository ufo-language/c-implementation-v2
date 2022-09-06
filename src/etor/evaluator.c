#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/string.h"
#include "data/symbol.h"
#include "data/triple.h"
#include "etor/evaluator.h"
#include "expr/exnhandler.h"
#include "expr/identifier.h"
#include "expr/savedenv.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "methods/methods.h"
#include "ns/all.h"

void threadManager_addThread(struct Evaluator* thread);
struct D_Symbol* threadManager_statusSymbol(enum ThreadStatus status);
void threadManager_terminateThread(struct Evaluator* thread);
void threadManager_unblockThread(struct Evaluator* thread);

struct Evaluator {
    struct Any obj;
    struct D_List* ostack;
    struct D_Triple* estack;
    struct D_Triple* env;
    struct D_HashTable* globalEnv;
    struct D_HashTable* recordNamespace;
    struct Any* exception;
    struct D_HashTable* subscriberTable;
    struct Any* blockingObject;
    struct D_Queue* waitingThreads;
    // NB: If you add an object field to this struct, you must add it to the _markChildren function below.
    enum ThreadStatus threadStatus;
    jmp_buf jumpBuf;
    bool showSteps;
    int tid;
};

struct Methods* evaluator_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_free = (void (*)(struct Any*))evaluator_free;
    methods->m_markChildren = (void (*)(struct Any* self))evaluator_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))evaluator_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))evaluator_sizeOf;
    methods->m_structSize = evaluator_structSize;
    return methods;
}

struct Evaluator* evaluator_new(void) {
    struct Evaluator* self = (struct Evaluator*)gc_alloc(T_Evaluator);
    evaluator_initialize(self);
    return self;
}

void evaluator_initialize(struct Evaluator* self) {
    static int nextTid = 0;
    self->ostack = EMPTY_LIST;
    self->estack = EMPTY_TRIPLE;
    self->env = EMPTY_TRIPLE;
    self->globalEnv = ns_all_globalEnv();
    self->recordNamespace = hashTable_new();
    self->exception = (struct Any*)NIL;
    self->subscriberTable = NULL;
    self->blockingObject = (struct Any*)NIL;
    self->waitingThreads = NULL;
    self->showSteps = false;
    self->threadStatus = TS_Dormant;
    self->tid = nextTid++;
}    

void evaluator_free(struct Evaluator* self) {
    free(self);
}

void evaluator_addWaitingThread(struct Evaluator* self, struct Evaluator* thread) {
    if (self->waitingThreads == NULL) {
        self->waitingThreads = queue_new();
    }
    queue_enq(self->waitingThreads, (struct Any*)thread);
}

struct D_Triple* evaluator_bind(struct Evaluator* self, struct E_Identifier* key, struct Any* value) {
    struct D_Triple* binding = triple_new((struct Any*)key, value, self->env);
    self->env = binding;
    return binding;
}

void evaluator_clearException(struct Evaluator* self) {
    self->exception = (struct Any*)NIL;
}

void evaluator_exit(struct Evaluator* self, int exitCode) {
    evaluator_throwException(
        self,
        symbol_new("NotImplemented"),
        "evaluator_exit() is not finished",
        (struct Any*)integer_new(exitCode)
    );
}

struct Any* evaluator_getBlockingObject(struct Evaluator* self) {
    return self->blockingObject;
}

struct D_Triple* evaluator_getEnv(struct Evaluator* self) {
    return self->env;
}

struct Any* evaluator_getException(struct Evaluator* self) {
    return self->exception;
}

struct D_HashTable* evaluator_getGlobalEnv(struct Evaluator* self) {
    return self->globalEnv;
}

struct D_Triple* evaluator_getEStack(struct Evaluator* self) {
    return self->estack;
}

struct D_List* evaluator_getOStack(struct Evaluator* self) {
    return self->ostack;
}

void evaluator_getJumpBuf(struct Evaluator* self, jmp_buf* jumpBuf) {
    memcpy(*jumpBuf, self->jumpBuf, sizeof(*jumpBuf));
}

struct D_HashTable* evaluator_getRecordNamespace(struct Evaluator* self) {
    return self->recordNamespace;
}

struct D_HashTable* evaluator_getSubscriberTable(struct Evaluator* self) {
    return self->subscriberTable;
}

enum ThreadStatus evaluator_getThreadStatus(struct Evaluator* self) {
    return self->threadStatus;
}

int evaluator_getTid(struct Evaluator* self) {
    return self->tid;
}

void evaluator_handleException(struct Evaluator* self) {
    while (!triple_isEmpty(self->estack)) {
        struct Any* expr = evaluator_popExpr(self);
        if (any_isA(expr, T_ExceptionHandler)) {
            struct E_ExceptionHandler* exnHandler = (struct E_ExceptionHandler*)expr;
            if (exceptionHandler_apply(exnHandler, self->exception, self)) {
                return;
            }
        }
    }
    fprintf(stderr, "ERROR: unhandled exception: ");
    any_show((struct Any*)self->exception, stderr);
    fputc('\n', stderr);
    struct D_Triple* env = self->env;
    evaluator_initialize(self);
    self->threadStatus = TS_Running;
    self->env = env;
    evaluator_pushObj(self, (struct Any*)NIL);
}

struct Any* evaluator_lookup(struct Evaluator* self, struct E_Identifier* key) {
    struct Any* value = triple_lookup(self->env, (struct Any*)key);
    if (value == NULL) {
        value = hashTable_get(self->globalEnv, (struct Any*)key);
    }
    return value;
}

void evaluator_markChildren(struct Evaluator* self) {
    any_mark((struct Any*)self->ostack);
    any_mark((struct Any*)self->estack);
    any_mark((struct Any*)self->env);
    any_mark((struct Any*)self->globalEnv);
    any_mark((struct Any*)self->recordNamespace);
    any_mark((struct Any*)self->exception);
    if (self->subscriberTable != NULL) {
        any_mark((struct Any*)self->subscriberTable);
    }
    any_mark((struct Any*)self->blockingObject);
    if (self->waitingThreads != NULL) {
        any_mark((struct Any*)self->waitingThreads);
    }
}

void evaluator_pushExpr(struct Evaluator* self, struct Any* expr) {
    self->estack = triple_new(expr, (struct Any*)self->env, (struct D_Triple*)self->estack);
}

void evaluator_pushExprEnv(struct Evaluator* self, struct Any* expr, struct Any* env) {
    self->estack = triple_new(expr, env, (struct D_Triple*)self->estack);
}

struct Any* evaluator_popExpr(struct Evaluator* self) {
#if defined(DEVELOPMENT)
    if (self->estack == NULL) {
        fprintf(stderr, "ERROR: %s: expression stack empty\n", __func__);
        exit(1);
    }
#endif
    struct Any* expr = (struct Any*)triple_getFirst(self->estack);
    struct D_Triple* env = (struct D_Triple*)triple_getSecond(self->estack);
    if (any_isA((struct Any*)env, T_Triple)) {
        // the env value is either a T_Triple or its T_Nil, which
        // means "use the dynamic environment"
        // TODO change to check for 'env == NIL'?
        self->env = env;
    }
    self->estack = (struct D_Triple*)triple_getNext(self->estack);
    return expr;
}

void evaluator_pushObj(struct Evaluator* self, struct Any* obj) {
#if defined(DEVELOPMENT)
    if (obj == NULL) {
        fprintf(stderr, "%s got null object\n", __func__);
        exit(1);
    }
#endif
    self->ostack = list_new(obj, (struct Any*)self->ostack);
}

struct Any* evaluator_popObj(struct Evaluator* self) {
#if defined(DEVELOPMENT)
    if (list_isEmpty(self->ostack)) {
        fprintf(stderr, "ERROR: %s: object stack empty\n", __func__);
        exit(1);
    }
#endif
    struct Any* obj = list_getFirst(self->ostack);
    self->ostack = (struct D_List*)list_getRest(self->ostack);
    return obj;
}

void evaluator_reassignBinding(struct Evaluator* self, struct E_Identifier* ident, struct Any* value) {
    struct D_Triple* binding = triple_locate(self->env, (struct Any*)ident);
    if (binding == NULL) {
        evaluator_throwException(
            self,
            symbol_new("Reassign"),
            "identifier not found",
            (struct Any*)ident
        );
    }
    triple_setSecond(binding, value);
}

void evaluator_runSteps(struct Evaluator* self, int nSteps) {
    self->threadStatus = TS_Running;
    if (setjmp(self->jumpBuf) > 0) {
        evaluator_handleException(self);
    }
    while (nSteps--) {
        gc_commit();
        if (GC_NEEDED) {
            gc_collect();
        }
        if (self->threadStatus != TS_Running) {
            return;
        }
        if (self->estack == EMPTY_TRIPLE) {
            threadManager_terminateThread(self);
            return;
        }
        struct Any* expr = evaluator_popExpr(self);
        if (self->showSteps) {  // TODO this is per-thread, I think it should be global instead
            printf("%s thread = %d, expr = ", __func__, self->tid);
            any_show(expr, stdout);
            printf(" :: %s\n", any_typeName(expr));
        }
        any_eval(expr, self);
    }
}

void evaluator_saveEnv(struct Evaluator* self) {
    if (self->estack != EMPTY_TRIPLE) {
        struct Any* topExpr = evaluator_topExpr(self);
        if (any_isA(topExpr, T_SavedEnv)) {
            return;
        }
    }
    struct Any* savedEnv = (struct Any*)savedEnv_new(self->env);
    evaluator_pushExpr(self, savedEnv);
}

void evaluator_setBlockingObject(struct Evaluator* self, struct Any* blockingObject) {
    self->blockingObject = blockingObject;
}

void evaluator_setEnv(struct Evaluator* self, struct D_Triple* env) {
    self->env = env;
}

void evaluator_setJumpBuf(struct Evaluator* self, jmp_buf* jumpBuf) {
    memcpy(self->jumpBuf, *jumpBuf, sizeof(*jumpBuf));
}

void evaluator_setShowSteps(struct Evaluator* self, bool showSteps) {
    self->showSteps = showSteps;
}

void evaluator_setSubscriberTable(struct Evaluator* self, struct D_HashTable* subscriberTable) {
    self->subscriberTable = subscriberTable;
}

void evaluator_setThreadStatus(struct Evaluator* self, enum ThreadStatus status) {
    self->threadStatus = status;
}

void evaluator_show(struct Evaluator* self, FILE* fp) {
    fprintf(fp, "Evaluator{tid=%d, status=", self->tid);
    symbol_show(threadManager_statusSymbol(self->threadStatus), fp);
    fprintf(fp, ", ostack=");
    any_show((struct Any*)self->ostack, fp);
    fputs(", estack=", fp);
    any_show((struct Any*)self->estack, fp);
    fputc('}', fp);
}

size_t evaluator_sizeOf(struct Evaluator* self) {
    return sizeof(self);
}

size_t evaluator_structSize(void) {
    return sizeof(struct Evaluator);
}

void evaluator_throwException(struct Evaluator* self, struct D_Symbol* symbol, char* message, struct Any* obj) {
    struct D_Array* ary = array_newN(3, symbol, string_new(message), obj);
    evaluator_throwExceptionObj(self, (struct Any*)ary);
}

void evaluator_throwExceptionObj(struct Evaluator* self, struct Any* exceptionObj) {
    self->exception = exceptionObj;
    longjmp(self->jumpBuf, 1);
}

struct Any* evaluator_topExpr(struct Evaluator* self) {
    struct Any* expr = triple_getFirst(self->estack);
    return expr;
}

void evaluator_unblockWaitingThreads(struct Evaluator* self) {
    struct D_Queue* threadQ = self->waitingThreads;
    if (threadQ != NULL) {
        for (int n=0; n<queue_count(threadQ); n++) {
            struct Evaluator* thread = (struct Evaluator*)queue_deq_unsafe(threadQ);
            threadManager_unblockThread(thread);
        }
    }
}
