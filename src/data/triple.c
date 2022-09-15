#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/hashtable.h"
#include "data/symbol.h"
#include "memory/gc.h"
#include "data/triple.h"
#include "methods/methods.h"

struct D_Triple {
    struct Any obj;
    struct Any* first;
    struct Any* second;
    struct D_Triple* next;
};

extern struct D_Symbol* GLOBAL_ENV;

struct Methods* triple_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_free = (void (*)(struct Any*))triple_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))triple_freeVars;
    methods->m_isEqual = (bool (*)(struct Any*, struct Any*))triple_isEqual;
    methods->m_markChildren = (void (*)(struct Any* self))triple_markChildren;
    methods->m_match = (struct D_Triple* (*)(struct Any*, struct Any*, struct D_Triple*))triple_match;
    methods->m_show = (void (*)(struct Any*, FILE*))triple_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))triple_sizeOf;
    methods->m_structSize = triple_structSize;
    return methods;
}

struct D_Triple* triple_new(struct Any* first, struct Any* second, struct D_Triple* next) {
    struct D_Triple* self = (struct D_Triple*)gc_alloc(T_Triple);
    self->first = first;
    self->second = second;
    self->next = next;
    return self;
}

void triple_free(struct D_Triple* self) {
    free(self);
}

void triple_freeVars(struct D_Triple* self, struct D_Set* freeVars, struct Evaluator* etor) {
    struct D_Triple* triple = self;
    while (!triple_isEmpty(triple)) {
        any_freeVars(triple->first, freeVars, etor);
        any_freeVars(triple->second, freeVars, etor);
        triple = triple->next;
    }
}

bool triple_isEmpty(struct D_Triple* self) {
    return self->first == NULL;
}

bool triple_isEqual(struct D_Triple* self, struct D_Triple* other) {
    while (!triple_isEmpty(self)) {
        if (!any_isEqual(self->first, other->first)) {
            return false;
        }
        if (!any_isEqual(self->second, other->second)) {
            return false;
        }
        self = self->next;
        other = other->next;
    }
    return triple_isEmpty(other);
}

struct Any* triple_getFirst(struct D_Triple* self) {
    return self->first;
}

struct Any* triple_getSecond(struct D_Triple* self) {
    return self->second;
}

struct D_Triple* triple_getNext(struct D_Triple* self) {
    return self->next;
}

struct D_Triple* triple_locate(struct D_Triple* self, struct Any* key) {
    while (!triple_isEmpty(self)) {
        if (any_isEqual(key, self->first)) {
            return self;
        }
        self = self->next;
    }
    return NULL;
}

struct Any* triple_lookup(struct D_Triple* self, struct Any* key) {
    struct D_Triple* triple = triple_locate(self, key);
    if (triple == NULL) {
        return NULL;
    }
    return triple->second;
}

void triple_markChildren(struct D_Triple* self) {
    if (!triple_isEmpty(self)) {
        any_mark(self->first);
        any_mark(self->second);
        any_mark((struct Any*)self->next);
    }
}

struct D_Triple* triple_match(struct D_Triple* self, struct Any* other, struct D_Triple* bindings) {
    if (((struct Any*)self)->typeId != other->typeId) {
        return NULL;
    }
    struct D_Triple* otherTriple = (struct D_Triple*)other;
    bindings = any_match(self->first, otherTriple->first, bindings);
    if (bindings == NULL) {
        return NULL;
    }
    bindings = any_match(self->second, otherTriple->second, bindings);
    if (bindings == NULL) {
        return NULL;
    }
    return any_match((struct Any*)self->next, (struct Any*)otherTriple->next, bindings);
}

void triple_setSecond(struct D_Triple* self, struct Any* second) {
    self->second = second;
}

void triple_show(struct D_Triple* self, FILE* fp) {
    triple_showWith(self, "[", ", ", "]", fp);
}

void triple_showWith(struct D_Triple* self, char* open, char* sep, char* close, FILE* fp) {
    fputs(open, fp);
    bool firstIter = true;
    while (!triple_isEmpty(self)) {
        if (firstIter) {
            firstIter = false;
        }
        else {
            fputs(sep, fp);
        }
        any_show(self->first, fp);
        fputc('=', fp);
        any_show(self->second, fp);
        self = (struct D_Triple*)self->next;
    }
    fputs(close, fp);
}

size_t triple_sizeOf(struct D_Triple* self) {
    (void)self;
    return sizeof(struct D_Triple);
}

size_t triple_structSize(void) {
    return sizeof(struct D_Triple);
}
