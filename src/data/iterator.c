#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/iterator.h"
#include "data/symbol.h"
#include "dispatch/methodtable.h"
#include "gc/gc.h"
#include "main/typedefs.h"
#include "utils/hash.h"

struct D_Iterator {
    struct Any obj;
};

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
    size_t           (*m_structSize)(void);
    struct Any*      (*m_typeOf)(struct Any* self);
};
#endif

struct Methods* iterator_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))iterator_boolValue;
    methods->m_free = (void (*)(struct Any*))iterator_free;
    methods->m_mark = (void (*)(struct Any*))iterator_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))iterator_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))iterator_sizeOf;
    methods->m_structSize = iterator_structSize;
    methods->m_typeOf = (struct Any* (*)(struct Any*))iterator_typeOf;
    return methods;
}

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

size_t iterator_structSize(enum TypeId typeId) {
    assert(T_Iterator == typeId);
    return sizeof(struct D_Iterator);
}

struct Any* iterator_typeOf(struct Any* self) {
    (void)self;
    char* typeName = TYPE_NAMES[T_Iterator];
    return (struct Any*)symbol_new(typeName);
}
