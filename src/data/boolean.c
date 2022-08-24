#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/boolean.h"
#include "dispatch/methodtable.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "main/typedefs.h"  // for HashCode
#include "utils/hash.h"

struct D_Boolean {
    struct Any obj;
    bool value;
};

bool boolean_boolValue(struct D_Boolean* self);
int boolean_compare(struct D_Boolean* self, struct D_Boolean* other, struct Evaluator* etor);
void boolean_free(struct D_Boolean* self);
bool boolean_hashCode(struct D_Boolean* self, HashCode* hashCode);
void boolean_show(struct D_Boolean* self, FILE* fp);
size_t boolean_sizeOf(struct D_Boolean* self);
size_t boolean_structSize(void);

struct Methods* boolean_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))boolean_boolValue;
    methods->m_compare = (int (*)(struct Any*, struct Any*, struct Evaluator* etor))boolean_compare;
    methods->m_free = (void (*)(struct Any*))boolean_free;
    methods->m_hashCode = (bool (*)(struct Any*, HashCode*))boolean_hashCode;
    methods->m_show = (void (*)(struct Any*, FILE*))boolean_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))boolean_sizeOf;
    methods->m_structSize = boolean_structSize;
    return methods;
}

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

bool boolean_hashCode(struct D_Boolean* self, HashCode* hashCode) {
    *hashCode = self->value ? HASH_PRIMES[T_Boolean] + 1
                            : HASH_PRIMES[T_Boolean];
    return true;
}

void boolean_show(struct D_Boolean* self, FILE* fp) {
    fputs(self == TRUE ? "true" : "false", fp);
}

size_t boolean_sizeOf(struct D_Boolean* self) {
    (void)self;
    return sizeof(struct D_Boolean);
}

size_t boolean_structSize(void) {
    return sizeof(struct D_Boolean);
}
