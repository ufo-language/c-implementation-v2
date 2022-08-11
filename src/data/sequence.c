#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/sequence.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "main/typedefs.h"
#include "utils/hash.h"

struct D_Sequence {
    int from;
    int to;
    int by;
};

struct D_Sequence* sequence_new(int from, int to, int by) {
    struct D_Sequence* self = (struct D_Sequence*)gc_alloc(T_Sequence);
    self->from = from;
    self->to = to;
    self->by = by;
    return self;
}

void sequence_free(struct D_Sequence* self) {
    free(self);
}

HashCode sequence_hashCode(struct D_Sequence* self, struct Evaluator* etor) {
    (void)etor;
    return (self->from + self->to + self->by) ^ HASH_PRIMES[T_Sequence];
}

bool sequence_isEqual(struct D_Sequence* self, struct D_Sequence* other) {
    return self->from == other->from
        && self->to == other->to
        && self->by == other->by;
}

void sequence_show(struct D_Sequence* self, FILE* fp) {
    fprintf(fp, "%d..%d", self->from, self->to);
    if (self->by != 1) {
        fprintf(fp, "%%%d", self->by);
    }
}

size_t sequence_sizeOf(struct D_Sequence* self) {
    (void)self;
    return sizeof(struct D_Sequence);
}

size_t sequence_structSize() {
    return sizeof(struct D_Sequence);
}
