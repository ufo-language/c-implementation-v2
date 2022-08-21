#include "data/array.h"
#include "data/symbol.h"
#include "dispatch/methodtable.h"
#include "etor/evaluator.h"

#if 0
struct Methods {
    bool             (*m_boolValue)(struct Any* self);
    int              (*m_compare)(struct Any* self, struct Any* other, struct Evaluator* etor);
    struct Any*      (*m_deepCopy)(struct Any* self);
    void             (*m_display)(struct Any* self, FILE* fp);
    void             (*m_eval)(struct Any* self, struct Evaluator* etor);
    void             (*m_free)(struct Any* self);
    void             (*m_freeVars)(struct Any* self, struct D_Set* freeVars, struct Evaluator* etor);
    HashCode         (*m_hashCode)(struct Any* self, struct Evaluator* etor);
    bool             (*m_isEqual)(struct Any* self, struct Any* other);
    void             (*m_mark)(struct Any* self);
    struct D_Triple* (*m_match)(struct Any* self, struct Any* other, struct D_Triple* bindings);
    void             (*m_show)(struct Any* self, FILE* fp);
    size_t           (*m_sizeOf)(struct Any* self);
    size_t           (*m_structSize)(enum TypeId typeId);
    struct Any*      (*m_typeOf)(struct Any* self);
};
#endif

struct Methods* METHOD_TABLE[T_FINAL + 1] = { NULL };

// These are the default functions for all the methods.

static bool m_boolValue(struct Any* self) {
    (void)self;
    return false;
}

static int m_compare(struct Any* self, struct Any* other, struct Evaluator* etor) {
    evaluator_throwException(
        etor,
        symbol_new("Compare"),
        "unable to comare objects",
        (struct Any*)array_newN(2, self, other)
    );
    return 0;
}

static struct Any* m_deepCopy(struct Any* self) {
    return self;
}

static void m_display(struct Any* self, FILE* fp) {
    any_show(self, fp);
}

static void m_eval(struct Any* self, struct Evaluator* etor) {
    evaluator_pushObj(etor, self);
}

static void m_free(struct Any* self) {
    fprintf(stderr, "ERROR: call to '%s' is not valid for type ID %d (%0x)\n", __func__, self->typeId, self->typeId);
    exit(1);
}

static void m_freeVars(struct Any* self, struct D_Set* freeVars, struct Evaluator* etor) {
    (void)self;
    (void)freeVars;
    (void)etor;
}

static HashCode m_hashCode(struct Any* self, struct Evaluator* etor) {
    evaluator_throwException(
        etor,
        symbol_new("HashCode"),
        "object is not hashable",
        self
    );
    return 0;
}

static bool m_isEqual(struct Any* self, struct Any* other) {
    (void)self;
    (void)other;
    return false;
}

static void m_mark(struct Any* self) {
    (void)self;
}

static struct D_Triple* m_match(struct Any* self, struct Any* other, struct D_Triple* bindings) {
    return any_isEqual(self, other) ? bindings : NULL;
}

static void m_show(struct Any* self, FILE* fp) {
    (void)fp;
    fprintf(stderr, "ERROR: call to '%s' is not valid for type ID %d (%0x)\n", __func__, self->typeId, self->typeId);
    exit(1);
}

static size_t m_sizeOf(struct Any* self) {
    fprintf(stderr, "ERROR: call to '%s' is not valid for type ID %d (%0x)\n", __func__, self->typeId, self->typeId);
    exit(1);
    return 0;
}

static size_t m_structSize(enum TypeId typeId) {
    fprintf(stderr, "ERROR: call to '%s' is not valid for type ID %d (%0x)\n", __func__, typeId, typeId);
    exit(1);
    return 0;
}

static struct Any* m_typeOf(struct Any* self) {
    fprintf(stderr, "ERROR: call to '%s' is not valid for type ID %d (%0x)\n", __func__, self->typeId, self->typeId);
    exit(1);
    return 0;
}

// This sets up a method table with default methods.
void methodTable_setupDefaults(struct Methods* methods) {
    methods->m_boolValue = m_boolValue;
    methods->m_compare = m_compare;
    methods->m_deepCopy = m_deepCopy;
    methods->m_display = m_display;
    methods->m_eval = m_eval;
    methods->m_free = m_free;
    methods->m_freeVars = m_freeVars;
    methods->m_hashCode = m_hashCode;
    methods->m_isEqual = m_isEqual;
    methods->m_mark = m_mark;
    methods->m_match = m_match;
    methods->m_show = m_show;
    methods->m_sizeOf = m_sizeOf;
    methods->m_structSize = m_structSize;
    methods->m_typeOf = m_typeOf;
}
