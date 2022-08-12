#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/integer.h"
#include "gc/gc.h"
#include "main/typedefs.h"
#include "utils/hash.h"

struct D_Integer {
    struct Any obj;
    int value;
};

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

HashCode integer_hashCode(struct D_Integer* self, struct Evaluator* etor) {
    (void)etor;
    return self->value ^ HASH_PRIMES[T_Integer];
}

bool integer_isEqual(struct D_Integer* self, struct D_Integer* other) {
    return self->value == other->value;
}

int integer_getValue(struct D_Integer* self) {
    return self->value;
}

void integer_show(struct D_Integer* self, FILE* fp) {
    fprintf(fp, "%d", ((struct D_Integer*)self)->value);
}

size_t integer_sizeOf(struct D_Integer* self) {
    (void)self;
    return sizeof(struct D_Integer);
}

size_t integer_structSize(void) {
    return sizeof(struct D_Integer);
}
