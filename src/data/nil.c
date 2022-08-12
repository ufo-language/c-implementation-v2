#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/nil.h"
#include "gc/gc.h"
#include "main/typedefs.h"
#include "utils/hash.h"

struct D_Nil {
    struct Any obj;
};

struct D_Nil* nil_new(void) {
    struct D_Nil* self = (struct D_Nil*)gc_alloc(T_Nil);
    return self;
}

void nil_free(struct D_Nil* self) {
    free(self);
}

int nil_compare(struct D_Nil* self, struct D_Nil* other, struct Evaluator* etor) {
    (void)self;
    (void)other;
    (void)etor;
    return 0;
}

HashCode nil_hashCode(struct D_Nil* self, struct Evaluator* etor) {
    (void)self;
    (void)etor;
    return HASH_PRIMES[T_Nil];
}

void nil_show(struct D_Nil* self, FILE* fp) {
    (void)self;
    fputs("nil", fp);
}

size_t nil_sizeOf(struct D_Nil* self) {
    (void)self;
    return sizeof(struct D_Nil);
}

size_t nil_structSize(void) {
    return sizeof(struct D_Nil);
}
