#include <stdio.h>
#include <stdlib.h>

#include "data/address.h"
#include "data/address.h"
#include "data/any.h"
#include "memory/gc.h"
#include "utils/hash.h"
#include "main/typedefs.h"  // for HashCode
#include "methods/methods.h"

struct D_Address* address_new(void* value) {
    struct D_Address* self = (struct D_Address*)gc_alloc(T_Address); 
    self->value = value;
    return self;
}

struct Methods* address_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))address_boolValue;
    methods->m_compare = (int (*)(struct Any*, struct Any*, struct Evaluator* etor))address_compare;
    methods->m_free = (void (*)(struct Any*))address_free;
    methods->m_hashCode = (bool (*)(struct Any*, HashCode*))address_hashCode;
    methods->m_isEqual = (bool (*)(struct Any*, struct Any*))address_isEqual;
    methods->m_show = (void (*)(struct Any*, FILE*))address_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))address_sizeOf;
    methods->m_structSize = address_structSize;
    return methods;
}

void address_free(struct D_Address* self) {
    free(self);
}

bool address_boolValue(struct D_Address* self) {
    return NULL != self->value;
}

int address_compare(struct D_Address* self, struct D_Address* other, struct Evaluator* etor) {
    (void)etor;
    if (self->value < other->value) {
        return -1;
    }
    if (self->value > other->value) {
        return 1;
    }
    return 0;
}

void* address_getValue(struct D_Address* self) {
    return self->value;
}

bool address_hashCode(struct D_Address* self, HashCode* hashCode) {
    union {
        void* address;
        // TODO does this field need padding so that it overlaps the
        // lower order bytes of the address field?
        HashCode hashCode;
    } u;
    u.address = self->value;
    *hashCode = u.hashCode ^ HASH_PRIMES[T_Address];
    return true;
}

bool address_isEqual(struct D_Address* self, struct D_Address* other) {
    return self->value == other->value;
}

void address_show(struct D_Address* self, FILE* fp) {
    fprintf(fp, "%p", ((struct D_Address*)self)->value);
}

size_t address_sizeOf(struct D_Address* self) {
    (void)self;
    return sizeof(struct D_Address);
}

size_t address_structSize(void) {
    return sizeof(struct D_Address);
}
