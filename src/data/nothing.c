#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/nothing.h"
#include "gc/gc.h"
#include "main/typedefs.h"
#include "utils/hash.h"

struct D_Nothing {
    struct Any obj;
};

struct D_Nothing* nothing_new() {
    struct D_Nothing* self = (struct D_Nothing*)gc_alloc(T_Nothing);
    return self;
}

void nothing_free(struct D_Nothing* self) {
    free(self);
}

int nothing_compare(struct D_Nothing* self, struct D_Nothing* other, struct Evaluator* etor) {
    (void)self;
    (void)other;
    (void)etor;
    return 0;
}

HashCode nothing_hashCode(struct D_Nothing* self, struct Evaluator* etor) {
    (void)self;
    (void)etor;
    return HASH_PRIMES[T_Nothing];
}

void nothing_show(struct D_Nothing* self, FILE* fp) {
    (void)self;
    fputs("nothing", fp);
}

size_t nothing_sizeOf(struct D_Nothing* self) {
    (void)self;
    return sizeof(struct D_Nothing);
}

size_t nothing_structSize() {
    return sizeof(struct D_Nothing);
}
