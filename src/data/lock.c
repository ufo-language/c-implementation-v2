#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/lock.h"
#include "data/any.h"
#include "data/queue.h"
#include "etor/evaluator.h"
#include "etor/threadmanager.h"
#include "expr/continuation.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "methods/methods.h"

struct D_Lock {
    struct Any obj;
    struct Evaluator* lockedBy;
    struct D_Queue* waitingThreads;
};

struct Any* lock_getPairValue(struct D_Lock* self, struct Any* key);

struct Methods* lock_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_free = (void (*)(struct Any*))lock_free;
    methods->m_markChildren = (void (*)(struct Any* self))lock_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))lock_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))lock_sizeOf;
    methods->m_structSize = lock_structSize;
    return methods;
}

struct D_Lock* lock_new(void) {
    struct D_Lock* self = (struct D_Lock*)gc_alloc(T_Lock);
    lock_initialize(self);
    return self;
}

void lock_initialize(struct D_Lock* self) {
    self->lockedBy = NULL;
    self->waitingThreads = queue_new();
}

void lock_acquire(struct D_Lock* self, struct Evaluator* etor) {
    if (self->lockedBy == NULL) {
        self->lockedBy = etor;
    }
    else {
        printf("%s blocking thread %d\n", __func__, evaluator_getTid(etor));
        threadManager_blockThread(etor, (struct Any*)self);
        queue_enq(self->waitingThreads, (struct Any*)etor);
    }
}

void lock_free(struct D_Lock* self) {
    free(self);
}

void lock_markChildren(struct D_Lock* self) {
    any_mark((struct Any*)self->waitingThreads);
    if (self->lockedBy != NULL) {
        any_mark((struct Any*)self->lockedBy);
    }
}

void lock_release(struct D_Lock* self) {
    self->lockedBy = NULL;
    if (!queue_isEmpty(self->waitingThreads)) {
        struct Evaluator* nextThread = (struct Evaluator*)queue_deq_unsafe(self->waitingThreads);
        threadManager_unblockThread(nextThread);
        printf("%s unblocking evaluator %d\n", __func__, evaluator_getTid(nextThread));
        lock_acquire(self, nextThread);
    }
}

void lock_show(struct D_Lock* self, FILE* fp) {
    (void)self;
    fputs("Lock{", fp);
    fputs(self->lockedBy == NULL ? "Unlocked" : "Locked", fp);
    fputc('}', fp);
}

size_t lock_sizeOf(struct D_Lock* self) {
    return sizeof(self);
}

size_t lock_structSize(void) {
    return sizeof(struct D_Lock);
}
