#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "main/globals.h"
#include "memory/gc.h"
#include "methods/methods.h"

struct D_Queue {
    struct Any obj;
    int count;
    struct D_List* head;
    struct D_List* tail;
};

struct Methods* queue_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))queue_boolValue;
    methods->m_compare = (int (*)(struct Any*, struct Any*, struct Evaluator* etor))queue_compare;
    methods->m_deepCopy = (struct Any* (*)(struct Any*))queue_deepCopy;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))queue_eval;
    methods->m_free = (void (*)(struct Any*))queue_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))queue_freeVars;
    methods->m_isEqual = (bool (*)(struct Any*, struct Any*))queue_isEqual;
    methods->m_iterator = (struct D_Iterator* (*)(struct Any*))queue_iterator;
    methods->m_markChildren = (void (*)(struct Any* self))queue_markChildren;
    methods->m_match = (struct D_Triple* (*)(struct Any*, struct Any*, struct D_Triple*))queue_match;
    methods->m_show = (void (*)(struct Any*, FILE*))queue_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))queue_sizeOf;
    methods->m_structSize = queue_structSize;
    return methods;
}

struct D_Queue* queue_new(void) {
    struct D_Queue* self = (struct D_Queue*)gc_alloc(T_Queue);
    self->head = EMPTY_LIST;
    self->tail = EMPTY_LIST;
    self->count = 0;
    return self;
}

void queue_free(struct D_Queue* self) {
    free(self);
}

struct D_List* queue_asList(struct D_Queue* self) {
    return self->head;
}

bool queue_boolValue(struct D_Queue* self) {
    return self->count != 0;
}

int queue_compare(struct D_Queue* self, struct D_Queue* other, struct Evaluator* etor) {
    return list_compare(self->head, other->head, etor);
}

int queue_count(struct D_Queue* self) {
    return self->count;
}

struct D_Queue* queue_deepCopy(struct D_Queue* self) {
    struct D_Queue* newQueue = queue_new();
    struct D_List* elems = self->head;
    while (!list_isEmpty(elems)) {
        queue_enq(newQueue, list_getFirst(elems));
        elems = (struct D_List*)list_getRest(elems);
    }
    return newQueue;
}

struct Any* queue_deq(struct D_Queue* self, struct Evaluator* etor) {
    struct Any* elem = queue_deq_unsafe(self);
    if (elem == NULL) {
        evaluator_throwException(
            etor,
            symbol_new("Queue"),
            "queue is empty",
            (struct Any*)self
        );
    }
    return elem;
}

struct Any* queue_deq_unsafe(struct D_Queue* self) {
    if (self->count == 0) {
        return NULL;
    }
    struct D_List* head = self->head;
    struct Any* elem = list_getFirst(head);
    struct D_List* rest = (struct D_List*)list_getRest(head);
    self->head = rest;
    self->count--;
    if (list_isEmpty(rest)) {
        self->tail = rest;
    }
    return elem;
}

void queue_enq(struct D_Queue* self, struct Any* elem) {
    struct D_List* list = list_new(elem, (struct Any*)EMPTY_LIST);
    if (self->count == 0) {
        self->head = list;
    }
    else {
        list_setRest(self->tail, (struct Any*)list);
    }
    self->tail = list;
    self->count++;    
}

static void _contin(struct E_Continuation* contin, struct Evaluator* etor) {
    struct Any* arg = continuation_getArg(contin);
    int count = integer_getValue((struct D_Integer*)arg);
    struct D_Queue* queue = queue_new();
    for (int n=0; n<count; n++) {
        struct Any* elem = evaluator_popObj(etor);
        queue_enq(queue, elem);
    }
    evaluator_pushObj(etor, (struct Any*)queue);
}

void queue_eval(struct D_Queue* self, struct Evaluator* etor) {
    struct D_Integer* countInt = integer_new(self->count);
    struct E_Continuation* contin = continuation_new(_contin, "Queue", (struct Any*)countInt);
    evaluator_pushExpr(etor, (struct Any*)contin);
    struct D_List* elems = self->head;
    for (int n=0; n<self->count; n++) {
        struct Any* elem = list_getFirst(elems);
        evaluator_pushExpr(etor, elem);
        elems = (struct D_List*)list_getRest(elems);
    }
}

void queue_freeVars(struct D_Queue* self, struct D_Set* freeVars, struct Evaluator* etor) {
    list_freeVars(self->head, freeVars, etor);
}

struct D_List* queue_getTail(struct D_Queue* self) {
    return self->tail;
}

bool queue_isEmpty(struct D_Queue* self) {
    return self->count == 0;
}

bool queue_isEqual(struct D_Queue* self, struct D_Queue* other) {
    return list_isEqual(self->head, other->head);
}

struct D_Iterator* queue_iterator(struct D_Queue* self) {
    return list_iterator(self->head);
}

void queue_markChildren(struct D_Queue* self) {
    any_mark((struct Any*)self->head);
}

struct D_Triple* queue_match(struct D_Queue* self, struct Any* other, struct D_Triple* bindings) {
    if (!any_isA(other, T_Queue)) {
        return NULL;
    }
    return list_match(self->head, (struct Any*)((struct D_Queue*)other)->head, bindings);
}

struct Any* queue_peek(struct D_Queue* self) {
    if (list_isEmpty(self->head)) {
        return NULL;
    }
    return list_getFirst(self->head);
}

struct Any* queue_peek_unsafe(struct D_Queue* self) {
    return list_getFirst(self->head);
}

void queue_show(struct D_Queue* self, FILE* fp) {
    list_showWith(self->head, "~[", ", ", "]", fp);
}

size_t queue_sizeOf(struct D_Queue* self) {
    (void)self;
    return sizeof(struct D_Queue);
}

size_t queue_structSize(void) {
    return sizeof(struct D_Queue);
}
