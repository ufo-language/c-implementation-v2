#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/iterator.h"
#include "data/symbol.h"
#include "dispatch/methodtable.h"
#include "gc/gc.h"
#include "main/typedefs.h"
#include "utils/hash.h"

struct D_Iterator {
    struct Any obj;
    struct Any* state;
    enum TypeId iteratorTypeId;
};

struct Methods* iterator_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))iterator_boolValue;
    methods->m_free = (void (*)(struct Any*))iterator_free;
    methods->m_markChildren = (void (*)(struct Any* self))iterator_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))iterator_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))iterator_sizeOf;
    methods->m_structSize = iterator_structSize;
    return methods;
}

struct D_Iterator* iterator_new(struct Any* state, enum TypeId iteratorTypeId) {
    struct D_Iterator* self = (struct D_Iterator*)gc_alloc(T_Iterator);
    self->state = state;
    self->iteratorTypeId = iteratorTypeId;
    return self;
}

void iterator_free(struct D_Iterator* self) {
    free(self);
}

bool iterator_boolValue(struct D_Iterator* self) {
    return METHOD_TABLE[self->iteratorTypeId]->m_iteratorHasNext(self);
}

struct Any* iterator_getStateObject(struct D_Iterator* self) {
    return self->state;
}

bool iterator_hasNext(struct D_Iterator* self) {
    return METHOD_TABLE[self->iteratorTypeId]->m_iteratorHasNext(self);
}

void iterator_markChildren(struct D_Iterator* self) {
    any_mark(self->state);
}

struct Any* iterator_next(struct D_Iterator* self) {
    return METHOD_TABLE[self->iteratorTypeId]->m_iteratorNext(self);
}

void iterator_setStateObject(struct D_Iterator* self, struct Any* state) {
    self->state = state;
}

void iterator_show(struct D_Iterator* self, FILE* fp) {
    fprintf(fp, "Iterator{%s, ", TYPE_NAMES[self->iteratorTypeId]);
    any_show(self->state, fp);
    fputc('}', fp);
}

size_t iterator_sizeOf(struct D_Iterator* self) {
    (void)self;
    return sizeof(struct D_Iterator);
}

size_t iterator_structSize(void) {
    return sizeof(struct D_Iterator);
}

struct Any* iterator_typeOf(struct Any* self) {
    (void)self;
    char* typeName = TYPE_NAMES[T_Iterator];
    return (struct Any*)symbol_new(typeName);
}
