#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/nil.h"
#include "dispatch/methodtable.h"
#include "gc/gc.h"
#include "main/typedefs.h"  // for HashCode
#include "utils/hash.h"

struct D_Nil {
    struct Any obj;
};

bool nil_boolValue(struct D_Nil* self);
int nil_compare(struct D_Nil* self, struct D_Nil* other, struct Evaluator* etor);
void nil_free(struct D_Nil* self);
bool nil_hashCode(struct D_Nil* self, HashCode* hashCode);
void nil_show(struct D_Nil* self, FILE* fp);
size_t nil_sizeOf(struct D_Nil* self);
size_t nil_structSize(void);

struct Methods* nil_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))nil_boolValue;
    methods->m_free = (void (*)(struct Any*))nil_free;
    methods->m_hashCode = (bool (*)(struct Any*, HashCode*))nil_hashCode;
    methods->m_show = (void (*)(struct Any*, FILE*))nil_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))nil_sizeOf;
    methods->m_structSize = nil_structSize;
    return methods;
}

struct D_Nil* nil_new(void) {
    struct D_Nil* self = (struct D_Nil*)gc_alloc(T_Nil);
    return self;
}

void nil_free(struct D_Nil* self) {
    free(self);
}

bool nil_boolValue(struct D_Nil* self) {
    (void)self;
    return false;
}

int nil_compare(struct D_Nil* self, struct D_Nil* other, struct Evaluator* etor) {
    (void)self;
    (void)other;
    (void)etor;
    return 0;
}

bool nil_hashCode(struct D_Nil* self, HashCode* hashCode) {
    (void)self;
    *hashCode = HASH_PRIMES[T_Nil];
    return true;
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
