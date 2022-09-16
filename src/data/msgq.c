#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/msgq.h"
#include "data/queue.h"
#include "etor/evaluator.h"
#include "etor/threadmanager.h"
#include "expr/continuation.h"
#include "main/globals.h"
#include "memory/gc.h"
#include "methods/methods.h"

struct D_MsgQ {
    struct Any obj;
    struct D_Queue* messages;
    struct D_Queue* waitingThreads;
};

static void _trigger(struct D_MsgQ* self);

struct Methods* msgq_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_free = (void (*)(struct Any*))msgq_free;
    methods->m_markChildren = (void (*)(struct Any* self))msgq_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))msgq_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))msgq_sizeOf;
    methods->m_structSize = msgq_structSize;
    return methods;
}

struct D_MsgQ* msgq_new(void) {
    struct D_MsgQ* self = (struct D_MsgQ*)gc_alloc(T_MsgQ);
    msgq_initialize(self);
    return self;
}

void msgq_initialize(struct D_MsgQ* self) {
    self->messages = queue_new();
    self->waitingThreads = queue_new();
}

int msgq_count(struct D_MsgQ* self) {
    return queue_count(self->messages);
}

void msgq_enq(struct D_MsgQ* self, struct Any* message) {
    queue_enq(self->messages, message);
    _trigger(self);
}

void msgq_free(struct D_MsgQ* self) {
    free(self);
}

void msgq_markChildren(struct D_MsgQ* self) {
    any_mark((struct Any*)self->waitingThreads);
    any_mark((struct Any*)self->messages);
}

void msgq_show(struct D_MsgQ* self, FILE* fp) {
    (void)self;
    fputs("MsgQ{", fp);
    queue_show(self->messages, fp);
    fputc('}', fp);
}

size_t msgq_sizeOf(struct D_MsgQ* self) {
    return sizeof(self);
}

size_t msgq_structSize(void) {
    return sizeof(struct D_MsgQ);
}

void msgq_wait(struct D_MsgQ* self, struct Evaluator* thread) {
    if (queue_isEmpty(self->messages)) {
        threadManager_blockThread(thread, (struct Any*)self);
        queue_enq(self->waitingThreads, (struct Any*)thread);
    }
    else {
        struct Any* message = queue_deq_unsafe(self->messages);
        evaluator_pushObj(thread, message);
    }
}

static void _trigger(struct D_MsgQ* self) {
    if (!queue_isEmpty(self->waitingThreads)) {
        struct Any* message = queue_deq_unsafe(self->messages);
        struct Evaluator* thread = (struct Evaluator*)queue_deq_unsafe(self->waitingThreads);
        evaluator_pushObj(thread, message);
        threadManager_unblockThread(thread);
    }    
}
