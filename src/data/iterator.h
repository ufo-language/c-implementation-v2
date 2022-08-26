#if !defined(ITERATOR_H)
#define ITERATOR_H

#include <stdio.h>

#include "main/typedefs.h"

struct D_Iterator;
struct Evaluator;

struct IteratorMethods {
    bool        (*m_boolValue)(struct D_Iterator*);
    struct Any* (*m_next)(struct D_Iterator*);
};

struct D_Iterator* iterator_new(struct Any* subtype, struct IteratorMethods* iteratorMethods);
void iterator_free(struct D_Iterator* self);

bool iterator_boolValue(struct D_Iterator* self);
struct Any* iterator_getSubtypeObject(struct D_Iterator* self);
void iterator_markChildren(struct D_Iterator* self);
void iterator_setSubtypeObject(struct D_Iterator* self, struct Any* object);
struct Any* iterator_next(struct D_Iterator* self);
void iterator_show(struct D_Iterator* self, FILE* fp);
size_t iterator_sizeOf(struct D_Iterator* self);
size_t iterator_structSize(void);
struct Any* iterator_typeOf(struct Any* self);

#endif
