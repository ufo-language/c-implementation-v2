#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/iterator.h"
#include "gc/gc.h"
#include "main/typedefs.h"
#include "utils/hash.h"

struct D_Iterator {
    struct Any obj;
};

struct D_Iterator* iterator_new(void) {
    struct D_Iterator* self = (struct D_Iterator*)gc_alloc(T_Iterator);
    return self;
}

void iterator_free(struct D_Iterator* self) {
    free(self);
}

bool iterator_boolValue(struct D_Iterator* self) {
    (void)self;
    printf("%s is incomplete", __func__);
    return false;
}

/*int iterator_compare(struct D_Iterator* self, struct D_Iterator* other, struct Evaluator* etor) {
    (void)self;
    (void)other;
    (void)etor;
    return 0;
}*/

/*HashCode iterator_hashCode(struct D_Iterator* self, struct Evaluator* etor) {
    (void)self;
    (void)etor;
    return HASH_PRIMES[T_Iterator];
}*/

void iterator_markChildren(struct D_Iterator* self) {
    (void)self;
    printf("%s is incomplete", __func__);
}

void iterator_show(struct D_Iterator* self, FILE* fp) {
    (void)self;
    fputs("iterator", fp);
}

size_t iterator_sizeOf(struct D_Iterator* self) {
    (void)self;
    return sizeof(struct D_Iterator);
}

size_t iterator_structSize(void) {
    return sizeof(struct D_Iterator);
}
