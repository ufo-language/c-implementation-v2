#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/boolean.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "main/typedefs.h"
#include "utils/hash.h"

struct D_Boolean {
    struct Any obj;
    bool value;
};

struct D_Boolean* boolean_from(bool value) {
    return value ? TRUE : FALSE;
}

struct D_Boolean* boolean_new(bool value) {
    struct D_Boolean* self = (struct D_Boolean*)gc_alloc(T_Boolean);
    self->value = value;
    return self;
}

void boolean_free(struct D_Boolean* self) {
    free(self);
}

bool boolean_boolValue(struct D_Boolean* self) {
    return self->value;
}
        
int boolean_compare(struct D_Boolean* self, struct D_Boolean* other, struct Evaluator* etor) {
    (void)etor;
    return (self->value == other->value) ? 0 : ((!self->value) ? -1 : 1);
}

HashCode boolean_hashCode(struct D_Boolean* self, struct Evaluator* etor) {
    (void)etor;
    return self->value ? HASH_PRIMES[T_Boolean] + 1
                       : HASH_PRIMES[T_Boolean];
}

void boolean_show(struct D_Boolean* self, FILE* fp) {
    fputs(self == TRUE ? "true" : "false", fp);
}

size_t boolean_sizeOf(struct D_Boolean* self) {
    (void)self;
    return sizeof(struct D_Boolean);
}

size_t boolean_structSize() {
    return sizeof(struct D_Boolean);
}
