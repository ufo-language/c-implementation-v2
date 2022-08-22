#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/tuple.h"
#include "dispatch/methodtable.h"
#include "etor/evaluator.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "main/typedefs.h"
#include "utils/hash.h"

struct D_Tuple {
    struct Any obj;
    int count;
    struct Any** elems;
};

struct Methods* tuple_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    //methods->m_compare = (int (*)(struct Any*, struct Any*, struct Evaluator* etor))tuple_compare;
    methods->m_deepCopy = (struct Any* (*)(struct Any*))tuple_deepCopy;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))tuple_eval;
    methods->m_free = (void (*)(struct Any*))tuple_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))tuple_freeVars;
    methods->m_hashCode = (HashCode (*)(struct Any*, struct Evaluator*))tuple_hashCode;
    methods->m_isEqual = (bool (*)(struct Any*, struct Any*))tuple_isEqual;
    methods->m_markChildren = (void (*)(struct Any* self))tuple_markChildren;
    methods->m_match = (struct D_Triple* (*)(struct Any*, struct Any*, struct D_Triple*))tuple_match;
    methods->m_show = (void (*)(struct Any*, FILE*))tuple_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))tuple_sizeOf;
    methods->m_structSize = tuple_structSize;
    return methods;
}

struct D_Tuple* tuple_new(int count) {
    struct D_Tuple* self = (struct D_Tuple*)gc_alloc(T_Tuple);
    struct Any** elems = (struct Any**)malloc(count * sizeof(struct Any*));
    self->count = count;
    self->elems = elems;
    for (int n=0; n<count; n++) {
        elems[n] = (struct Any*)NIL;
    }
    return self;
}

struct D_Tuple* tuple_newN(int count, ...) {
    va_list argList;
    va_start(argList, count);
    struct D_Tuple* self = tuple_new(count);
    for (int n=0; n<count; n++) {
        struct Any* elem = va_arg(argList, struct Any*);
        self->elems[n] = elem;
    }
    va_end(argList);
    return self;
}

void tuple_free(struct D_Tuple* self) {
    free(self->elems);
    free(self);
}

int tuple_count(struct D_Tuple* self) {
    return self->count;
}

struct D_Tuple* tuple_deepCopy(struct D_Tuple* self) {
    struct D_Tuple* newTuple = tuple_new(self->count);
    for (int n=0; n<self->count; n++) {
        newTuple->elems[n] = self->elems[n];
    }
    return newTuple;
}

void tuple_eval(struct D_Tuple* self, struct Evaluator* etor) {
    fprintf(stderr, "WARNING: %s is not implemented\n", __func__);
    evaluator_pushObj(etor, (struct Any*)self);
}

void tuple_freeVars(struct D_Tuple* self, struct D_Set* freeVars, struct Evaluator* etor) {
    for (int n=0; n<self->count; n++) {
        any_freeVars(self->elems[n], freeVars, etor);
    }
}

struct Any* tuple_get_unsafe(struct D_Tuple* self, int n) {
    return self->elems[n];
}

HashCode tuple_hashCode(struct D_Tuple* self, struct Evaluator* etor) {
    (void)etor;
    HashCode hashCode = 0;
    for (int n=0; n<self->count; n++) {
        hashCode = hashRotateLeft(hashCode) + any_hashCode(self->elems[n], etor);
    }
    return hashCode ^ HASH_PRIMES[T_Tuple];
}

bool tuple_isEqual(struct D_Tuple* self, struct D_Tuple* other) {
    if (self->count != other->count) {
        return false;
    }
    for (int n=0; n<self->count; n++) {
        if (!any_isEqual(self->elems[n], other->elems[n])) {
            return false;
        }
    }
    return true;
}

void tuple_markChildren(struct D_Tuple* self) {
    for (int n=0; n<self->count; n++) {
        any_mark(self->elems[n]);
    }
}

struct D_Triple* tuple_match(struct D_Tuple* self, struct Any* other, struct D_Triple* bindings) {
    if (((struct Any*)self)->typeId != other->typeId) {
        return NULL;
    }
    struct D_Tuple* otherTuple = (struct D_Tuple*)other;
    if (self->count != otherTuple->count) {
        return NULL;
    }
    for (int n=0; n<self->count; n++) {
        bindings = any_match(self->elems[n], otherTuple->elems[n], bindings);
        if (bindings == NULL) {
            return NULL;
        }
    }
    return bindings;
}

void tuple_show(struct D_Tuple* self, FILE* fp) {
    for (int n=0; n<self->count; n++) {
        if (n != 0) {
            fputc(':', fp);
        }
        any_show(self->elems[n], fp);
    }
}

size_t tuple_sizeOf(struct D_Tuple* self) {
    return sizeof(self) + sizeof(self->elems);
}

size_t tuple_structSize(void) {
    return sizeof(struct D_Tuple);
}
