#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/unsigned.h"
#include "gc/gc.h"
#include "main/typedefs.h"  // for HashCode
#include "methods/methods.h"
#include "utils/hash.h"

struct D_Unsigned {
    struct Any obj;
    size_t value;
};

struct Methods* unsigned_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))unsigned_boolValue;
    methods->m_compare = (int (*)(struct Any*, struct Any*, struct Evaluator* etor))unsigned_compare;
    methods->m_free = (void (*)(struct Any*))unsigned_free;
    methods->m_hashCode = (bool (*)(struct Any*, HashCode*))unsigned_hashCode;
    methods->m_isEqual = (bool (*)(struct Any*, struct Any*))unsigned_isEqual;
    methods->m_show = (void (*)(struct Any*, FILE*))unsigned_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))unsigned_sizeOf;
    methods->m_structSize = unsigned_structSize;
    return methods;
}

struct D_Unsigned* unsigned_new(size_t value) {
    struct D_Unsigned* self = (struct D_Unsigned*)gc_alloc(T_Unsigned);
    unsigned_initialize(self, value);
    return self;
}

void unsigned_display(struct D_Unsigned* self, FILE* fp) {
    fprintf(fp, "%lu", (unsigned long)self->value);
}

void unsigned_initialize(struct D_Unsigned* self, size_t value) {
    self->value = value;
}

void unsigned_free(struct D_Unsigned* self) {
    free(self);
}

bool unsigned_boolValue(struct D_Unsigned* self) {
    return self->value != 0;
}

int unsigned_compare(struct D_Unsigned* self, struct D_Unsigned* other, struct Evaluator* etor) {
    (void)etor;
    return (self->value < other->value) ? -1 : ((self->value > other->value) ? 1 : 0);
}

bool unsigned_hashCode(struct D_Unsigned* self, HashCode* hashCode) {
    *hashCode = self->value ^ HASH_PRIMES[T_Unsigned];
    return true;
}

bool unsigned_isEqual(struct D_Unsigned* self, struct D_Unsigned* other) {
    return self->value == other->value;
}

size_t unsigned_getValue(struct D_Unsigned* self) {
    return self->value;
}

void unsigned_show(struct D_Unsigned* self, FILE* fp) {
    fprintf(fp, "%luU", ((struct D_Unsigned*)self)->value);
}

size_t unsigned_sizeOf(struct D_Unsigned* self) {
    (void)self;
    return sizeof(struct D_Unsigned);
}

size_t unsigned_structSize(void) {
    return sizeof(struct D_Unsigned);
}
