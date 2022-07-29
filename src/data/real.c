#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/real.h"
#include "gc/gc.h"

struct D_Real {
    struct Any obj;
    double value;
};

struct D_Real* real_new(double value) {
    struct D_Real* self = (struct D_Real*)gc_alloc(T_Real);
    self->value = value;
    return self;
}

void real_free(struct D_Real* self) {
    free(self);
}

bool real_boolValue(struct D_Real* self) {
    return self->value != 0.0;
}

int real_compare(struct D_Real* self, struct D_Real* other, struct Evaluator* etor) {
    (void)etor;
    return (self->value < other->value) ? -1 : ((self->value > other->value) ? 1 : 0);
}

bool real_isEqual(struct D_Real* self, struct D_Real* other) {
    return self->value == other->value;
}

double real_getValue(struct D_Real* self) {
    return self->value;
}

void real_show(struct D_Real* self, FILE* fp) {
    // %g prints a double without trailing zeros
    fprintf(fp, "%.15g", ((struct D_Real*)self)->value);
}

size_t real_sizeOf(struct D_Real* self) {
    (void)self;
    return sizeof(struct D_Real);
}

size_t real_structSize() {
    return sizeof(struct D_Real);
}
