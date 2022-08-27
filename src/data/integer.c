#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/integer.h"
#include "data/iterator.h"
#include "data/list.h"
#include "dispatch/methodtable.h"
#include "gc/gc.h"
#include "main/typedefs.h"  // for HashCode
#include "utils/hash.h"

struct D_Integer {
    struct Any obj;
    int value;
};

bool integer_iteratorHasNext(struct D_Iterator* iterator);
struct Any* integer_iteratorNext(struct D_Iterator* iterator);

struct Methods* integer_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))integer_boolValue;
    methods->m_compare = (int (*)(struct Any*, struct Any*, struct Evaluator* etor))integer_compare;
    methods->m_free = (void (*)(struct Any*))integer_free;
    methods->m_hashCode = (bool (*)(struct Any*, HashCode*))integer_hashCode;
    methods->m_isEqual = (bool (*)(struct Any*, struct Any*))integer_isEqual;
    methods->m_iterator = (struct D_Iterator* (*)(struct Any*))integer_iterator;
    methods->m_iteratorHasNext = integer_iteratorHasNext;
    methods->m_iteratorNext = integer_iteratorNext;
    methods->m_show = (void (*)(struct Any*, FILE*))integer_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))integer_sizeOf;
    methods->m_structSize = integer_structSize;
    return methods;
}

struct D_Integer* integer_new(int value) {
    struct D_Integer* self = (struct D_Integer*)gc_alloc(T_Integer);
    integer_initialize(self, value);
    return self;
}

void integer_initialize(struct D_Integer* self, int value) {
    self->value = value;
}

void integer_free(struct D_Integer* self) {
    free(self);
}

bool integer_boolValue(struct D_Integer* self) {
    return self->value != 0;
}

int integer_compare(struct D_Integer* self, struct D_Integer* other, struct Evaluator* etor) {
    (void)etor;
    return (self->value < other->value) ? -1 : ((self->value > other->value) ? 1 : 0);
}

int integer_getValue(struct D_Integer* self) {
    return self->value;
}

bool integer_hashCode(struct D_Integer* self, HashCode* hashCode) {
    *hashCode = self->value;
    return true;
}

bool integer_isEqual(struct D_Integer* self, struct D_Integer* other) {
    return self->value == other->value;
}

struct D_Iterator* integer_iterator(struct D_Integer* self) {
    struct D_List* pair = list_new((struct Any*)integer_new(0), (struct Any*)self);
    return iterator_new((struct Any*)pair, T_Integer);
}

bool integer_iteratorHasNext(struct D_Iterator* iterator) {
    struct D_List* iterObj = (struct D_List*)iterator_getStateObject(iterator);
    struct D_Integer* indexObj = (struct D_Integer*)list_getFirst(iterObj);
    struct D_Integer* maxObj = (struct D_Integer*)list_getRest(iterObj);
    return indexObj->value < maxObj->value;
}

struct Any* integer_iteratorNext(struct D_Iterator* iterator) {
    struct D_List* iterObj = (struct D_List*)iterator_getStateObject(iterator);
    struct D_Integer* indexObj = (struct D_Integer*)list_getFirst(iterObj);
    struct D_Integer* maxObj = (struct D_Integer*)list_getRest(iterObj);
    int index = indexObj->value;
    if (index < maxObj->value) {
        list_setFirst(iterObj, (struct Any*)integer_new(index + 1));
        return (struct Any*)indexObj;
    }
    return NULL;
}

void integer_show(struct D_Integer* self, FILE* fp) {
    fprintf(fp, "%d", ((struct D_Integer*)self)->value);
}

size_t integer_sizeOf(struct D_Integer* self) {
    (void)self;
    return sizeof(struct D_Integer);
}

size_t integer_structSize() {
    return sizeof(struct D_Integer);
}
