#include <stdbool.h>

#include "data/array.h"
#include "data/list.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "main/globals.h"
#include "methods/methods.h"

struct D_Stream;

struct Methods* METHOD_TABLE[T_FINAL + 1] = { NULL };

// These are the default functions for all the methods.

static bool m_boolValue(struct Any* self) {
    (void)self;
    return false;
}

static int m_compare(struct Any* self, struct Any* other, struct Evaluator* etor) {
    evaluator_throwException(
        etor,
        symbol_new("Compare"),
        "unable to comare objects",
        (struct Any*)array_newN(2, self, other)
    );
    return 0;
}

static struct Any* m_deepCopy(struct Any* self) {
    return self;
}

static void m_display(struct Any* self, FILE* fp) {
    any_show(self, fp);
}

static void m_eval(struct Any* self, struct Evaluator* etor) {
    evaluator_pushObj(etor, self);
}

static void m_free(struct Any* self) {
    fprintf(stderr, "ERROR: call to '%s' is not valid for type ID %d (%0x)\n", __func__, self->typeId, self->typeId);
    exit(1);
}

static void m_freeVars(struct Any* self, struct D_Set* freeVars, struct Evaluator* etor) {
    (void)self;
    (void)freeVars;
    (void)etor;
}


static struct Any* m_getPairValue(struct Any* obj, struct Any* key) {
    (void)obj;
    (void)key;
    return NULL;
}

static bool m_hashCode(struct Any* self, HashCode* hashCode) {
    (void)self;
    (void)hashCode;
    return false;
}

static bool m_isEqual(struct Any* self, struct Any* other) {
    (void)self;
    (void)other;
    return false;
}

struct D_Iterator* m_iterator(struct Any* self) {
    return list_iterator(list_new(self, (struct Any*)EMPTY_LIST));
}

bool m_iteratorHasNext(struct D_Iterator* iter) {
    (void)iter;
    return false;
}

struct Any* m_iteratorNext(struct D_Iterator* iter) {
    (void)iter;
    return (struct Any*)NIL;
}

static void m_markChildren(struct Any* self) {
    (void)self;
}

static struct D_Triple* m_match(struct Any* self, struct Any* other, struct D_Triple* bindings) {
    return any_isEqual(self, other) ? bindings : NULL;
}

static void m_show(struct Any* self, FILE* fp) {
    (void)fp;
    fprintf(stderr, "ERROR: call to '%s' is not valid for type ID %d (%0x)\n", __func__, self->typeId, self->typeId);
    exit(1);
}

static size_t m_sizeOf(struct Any* self) {
    fprintf(stderr, "ERROR: call to '%s' is not valid for type ID %d (%0x)\n", __func__, self->typeId, self->typeId);
    exit(1);
    return 0;
}

static bool m_streamReadChar(struct Any* stream, char* c) {
    (void)stream;
    (void)c;
    return false;
}

static bool m_streamWriteChar(struct Any* stream, char c) {
    (void)stream;
    (void)c;
    return false;
}

static size_t m_structSize(void) {
    fprintf(stderr, "ERROR: call to '%s' is not valid, no TypeId information\n", __func__);
    exit(1);
    return 0;
}

static struct Any* m_typeOf(struct Any* self) {
    char* typeName = TYPE_NAMES[self->typeId];
    return (struct Any*)symbol_new(typeName);
}

// This sets up a method table with default methods.
void methodTable_setupDefaults(struct Methods* methods) {
    methods->m_boolValue = m_boolValue;
    methods->m_compare = m_compare;
    methods->m_deepCopy = m_deepCopy;
    methods->m_display = m_display;
    methods->m_eval = m_eval;
    methods->m_free = m_free;
    methods->m_freeVars = m_freeVars;
    methods->m_getPairValue = m_getPairValue;
    methods->m_hashCode = m_hashCode;
    methods->m_isEqual = m_isEqual;
    methods->m_iterator = m_iterator;
    methods->m_iteratorHasNext = m_iteratorHasNext;
    methods->m_iteratorNext = m_iteratorNext;
    methods->m_markChildren = m_markChildren;
    methods->m_match = m_match;
    methods->m_show = m_show;
    methods->m_sizeOf = m_sizeOf;
    methods->m_streamReadChar = m_streamReadChar;
    methods->m_streamWriteChar = m_streamWriteChar;
    methods->m_structSize = m_structSize;
    methods->m_typeOf = m_typeOf;
}
