#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "data/iterator.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "methods/methods.h"

struct D_List {
    struct Any obj;
    struct Any* first;
    struct Any* rest;
};

struct Any* list_getPairValue(struct D_List* self, struct Any* key);
bool list_iteratorHasNext(struct D_Iterator* iterator);
struct Any* list_iteratorNext(struct D_Iterator* iterator);

struct Methods* list_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))list_boolValue;
    methods->m_compare = (int (*)(struct Any*, struct Any*, struct Evaluator* etor))list_compare;
    methods->m_deepCopy = (struct Any* (*)(struct Any*))list_deepCopy;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))list_eval;
    methods->m_free = (void (*)(struct Any*))list_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))list_freeVars;
    methods->m_getPairValue = (struct Any* (*)(struct Any*, struct Any*))list_getPairValue;
    methods->m_isEqual = (bool (*)(struct Any*, struct Any*))list_isEqual;
    methods->m_iterator = (struct D_Iterator* (*)(struct Any*))list_iterator;
    methods->m_iteratorHasNext = list_iteratorHasNext;
    methods->m_iteratorNext = list_iteratorNext;
    methods->m_markChildren = (void (*)(struct Any* self))list_markChildren;
    methods->m_match = (struct D_Triple* (*)(struct Any*, struct Any*, struct D_Triple*))list_match;
    methods->m_show = (void (*)(struct Any*, FILE*))list_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))list_sizeOf;
    methods->m_structSize = list_structSize;
    return methods;
}

struct D_List* list_new(struct Any* first, struct Any* rest) {
    struct D_List* self = (struct D_List*)gc_alloc(T_List);
    self->first = first;
    self->rest = rest;
    return self;
}

struct D_List* list_from(struct Any* arg, ...) {
    struct D_List* head = EMPTY_LIST;
    struct D_List* tail = EMPTY_LIST;
    va_list argList;
    va_start(argList, arg);
    while (arg != NULL) {
        struct D_List* link = list_new(arg, (struct Any*)EMPTY_LIST);
        if (list_isEmpty(head)) {
            head = tail = link;
        }
        else {
            tail->rest = (struct Any*)link;
            tail = link;
        }
        arg = va_arg(argList, struct Any*);
    }
    va_end(argList);
    return head;
}

void list_free(struct D_List* self) {
    free(self);
}

struct D_Array* list_asArray(struct D_List* self) {
    int count = list_count(self);
    struct D_Array* ary = array_new(count);
    struct D_List* list = self;
    int n = 0;
    while (!list_isEmpty(list)) {
        array_set_unsafe(ary, n++, list->first);
        struct Any* rest = list->rest;
        if (!any_isA(rest, T_List)) {
            array_set_unsafe(ary, n, list->rest);
            break;;
        }
        list = (struct D_List*)rest;
    }
    return ary;
}

bool list_boolValue(struct D_List* self) {
    return !list_isEmpty(self);
}

int list_compare(struct D_List* self, struct D_List* other, struct Evaluator* etor) {
    bool selfEmpty = list_isEmpty(self);
    bool otherEmpty = list_isEmpty(other);
    if (selfEmpty && otherEmpty) {
        return 0;
    }
    if (selfEmpty) {
        return -1;
    }
    if (otherEmpty) {
        return 1;
    }
    int res = any_compare(self->first, other->first, etor);
    if (res == 0) {
        res = any_compare(self->rest, other->rest, etor);
    }
    return res;
}

int list_count(struct D_List* self) {
    struct D_List* list = self;
    int count = 0;
    while (!list_isEmpty(list)) {
        count++;
        struct Any* rest = list->rest;
        if (!any_isA(rest, T_List)) {
            return count + 1;
        }
        list = (struct D_List*)rest;
    }
    return count;
}

static void list_contin(struct Evaluator* etor, struct Any* arg) {
    (void)arg;
    struct Any* rest = evaluator_popObj(etor);
    struct Any* first = evaluator_popObj(etor);
    struct D_List* list = list_new(first, rest);
    evaluator_pushObj(etor, (struct Any*)list);
}

struct D_List* list_deepCopy(struct D_List* self) {
    if (list_isEmpty(self)) {
        return self;
    }
    return list_new(any_deepCopy(self->first), any_deepCopy(self->rest));
}

void list_eval(struct D_List* self, struct Evaluator* etor) {
    if (list_isEmpty(self)) {
        evaluator_pushObj(etor, (struct Any*)self);
    }
    else {
        evaluator_pushExpr(etor, (struct Any*)continuation_new(list_contin, "list", (struct Any*)NIL));
        evaluator_pushExpr(etor, self->rest);
        evaluator_pushExpr(etor, self->first);
    }
}

void list_freeVars(struct D_List* self, struct D_Set* freeVars, struct Evaluator* etor) {
    if (!list_isEmpty(self)) {
        any_freeVars(self->first, freeVars, etor);
        any_freeVars(self->rest, freeVars, etor);
    }
}

struct Any* list_getFirst(struct D_List* self) {
    return self->first;
}

struct Any* list_getPairValue(struct D_List* self, struct Any* key) {
    if (list_isEmpty(self)) {
        return NULL;
    }
    return any_isEqual(key, self->first) ? self->rest : NULL;
}

struct Any* list_getRest(struct D_List* self) {
    return self->rest;
}

struct D_List* list_getRestAsList(struct D_List* self) {
    if (T_List == any_typeId(self->rest)) {
        return (struct D_List*)self->rest;
    }
    return list_new(self->rest, (struct Any*)EMPTY_LIST);
}

struct Any* list_getSecond(struct D_List* self) {
    if (any_isA(self->rest, T_List)) {
        return ((struct D_List*)self->rest)->first;
    }
    return NULL;
}

struct Any* list_getNth(struct D_List* self, int n, struct Evaluator* etor) {
    struct D_List* list = self;
    bool error = false;
    for (int m=0; m<n; m++) {
        if (list_isEmpty(list)) {
            error = true;
            break;
        }
        struct Any* rest = list_getRest(list);
        if (!any_isA(rest, T_List)) {
            error = true;
            break;
        }
        list = (struct D_List*)rest;
    }
    if (error || list_isEmpty(list)) {
        evaluator_throwException(
            etor,
            symbol_new("List"),
            "nth element of list doesn't exist",
            (struct Any*)array_newN(2, (struct Any*)integer_new(n), (struct Any*)self)
        );
    }
    return list_getFirst(list);
}

bool list_isEmpty(struct D_List* self) {
    return self->first == NULL;
}

void list_markChildren(struct D_List* self) {
    if (!list_isEmpty(self)) {
        any_mark(self->first);
        any_mark(self->rest);
    }
}

bool list_isEqual(struct D_List* self, struct D_List* other) {
    while (!list_isEmpty(self)) {
        if (!any_isEqual(self->first, other->first)) {
            return false;
        }
        if (self->rest->typeId == T_List) {
            if (other->rest->typeId == T_List) {
                self = (struct D_List*)self->rest;
                other = (struct D_List*)self->rest;
            }
            else {
                return false;
            }
        }
        else {
            return any_isEqual(self->rest, other->rest);
        }
    }
    return list_isEmpty(other);
}

struct D_Iterator* list_iterator(struct D_List* self) {
    return iterator_new((struct Any*)self, T_List);
}

bool list_iteratorHasNext(struct D_Iterator* iterator) {
    struct D_List* list = (struct D_List*)iterator_getStateObject(iterator);
    return list_boolValue(list);
}

struct Any* list_iteratorNext(struct D_Iterator* iterator) {
    struct D_List* list = (struct D_List*)iterator_getStateObject(iterator);
    if (!list_boolValue(list)) {
        return NULL;
    }
    struct Any* rest;
    if (T_List != any_typeId(list->rest)) {
        rest = (struct Any*)list_new(list->rest, (struct Any*)EMPTY_LIST);
    }
    else {
        rest = list->rest;
    }
    iterator_setStateObject(iterator, rest);
    return list->first;
}

// TODO This is written recursively. Rewrite it to be iterative like the isEqual function.
struct D_Triple* list_match(struct D_List* self, struct Any* other, struct D_Triple* bindings) {
    if (!any_isA(other, T_List)) {
        return NULL;
    }
    struct D_List* otherList = (struct D_List*)other;
    if (list_isEmpty(self)) {
        return list_isEmpty(otherList) ? bindings: NULL;
    }
    if (list_isEmpty(otherList)) {
        return NULL;
    }
    bindings = any_match(self->first, otherList->first, bindings);
    if (bindings == NULL) {
        return NULL;
    }
    return any_match(self->rest, otherList->rest, bindings);
}

void list_setFirst(struct D_List* self, struct Any* first) {
    self->first = first;
}

void list_setRest(struct D_List* self, struct Any* rest) {
    self->rest = rest;
}

void list_show(struct D_List* self, FILE* fp) {
    list_showWith(self, "[", ", ", "]", fp);
}

void list_showWith(struct D_List* self, char* open, char* sep, char* close, FILE* fp) {
    fputs(open, fp);
    bool firstIter = true;
    while (!list_isEmpty(self)) {
        if (firstIter) {
            firstIter = false;
        }
        else {
            fputs(sep, fp);
        }
        any_show(self->first, fp);
        if (self->rest->typeId == T_List) {
            self = (struct D_List*)self->rest;
        }
        else {
            fputs(" | ", fp);
            any_show(self->rest, fp);
            break;
        }
    }
    fputs(close, fp);
}

size_t list_sizeOf(struct D_List* self) {
    return sizeof(self);
}

size_t list_structSize(void) {
    return sizeof(struct D_List);
}

// This is called only by the JSON parser.
struct D_Array* list_toArray(struct D_List* self, int nElems) {
    struct D_Array* ary = array_new(nElems);
    for (int n=0; n<nElems; n++) {
        array_set_unsafe(ary, n, self->first);
        self = (struct D_List*)self->rest;
    }
    return ary;
}

#if 0
void _each(struct D_List* self, void (*fun)(struct Any*)) {
    struct D_List* list = self;
    while (!list_isEmpty(list)) {
        fun(list->first);
        struct Any* rest = list->rest;
        if (!any_isA(rest, T_List)) {
            fun(list->first);
            break;;
        }
        list = (struct D_List*)rest;
    }
}

struct D_List* _map(struct D_List* self, struct Any* (*fun)(struct Any*)) {
    struct D_List* list = self;
    struct D_Queue* q = queue_new();
    while (!list_isEmpty(list)) {
        struct Any* newElem = fun(list->first);
        queue_enq(q, newElem);
        struct Any* rest = list->rest;
        if (!any_isA(rest, T_List)) {
            struct Any* newElem = fun(list->first);
            queue_enq(q, newElem);
            break;;
        }
        list = (struct D_List*)rest;
    }
    return queue_asList(q);;
}
#endif
