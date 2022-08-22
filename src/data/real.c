#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/real.h"
#include "gc/gc.h"

struct D_Real {
    struct Any obj;
    double value;
};

bool real_boolValue(struct D_Real* self);
int real_compare(struct D_Real* self, struct D_Real* other, struct Evaluator* etor);
void real_free(struct D_Real* self);
bool real_isEqual(struct D_Real* self, struct D_Real* other);
void real_show(struct D_Real* self, FILE* fp);
size_t real_sizeOf(struct D_Real* self);
size_t real_structSize(void);

struct Methods* real_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))real_boolValue;
    methods->m_compare = (int (*)(struct Any*, struct Any*, struct Evaluator* etor))real_compare;
    methods->m_free = (void (*)(struct Any*))real_free;
    methods->m_isEqual = (bool (*)(struct Any*, struct Any*))real_isEqual;
    methods->m_show = (void (*)(struct Any*, FILE*))real_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))real_sizeOf;
    methods->m_structSize = real_structSize;
    return methods;
}

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

size_t real_structSize(void) {
    return sizeof(struct D_Real);
}
