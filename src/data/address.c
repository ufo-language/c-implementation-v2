#include <stdio.h>
#include <stdlib.h>

#include "data/address.h"
#include "data/address.h"
#include "data/any.h"
#include "gc/gc.h"
#include "utils/hash.h"
#include "main/typedefs.h"

struct D_Address* address_new(void* value) {
    struct D_Address* self = (struct D_Address*)gc_alloc(T_Address); 
    self->value = value;
    return self;
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

HashCode address_hashCode(struct D_Address* self, struct Evaluator* etor) {
    (void)etor;
    union {
        void* address;
        // TODO does this field need padding so that it overlaps the
        // lower order bytes of the address field?
        HashCode hashCode;
    } u;
    u.address = self->value;
    return u.hashCode ^ HASH_PRIMES[T_Address];
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

size_t address_structSize() {
    return sizeof(struct D_Address);
}
