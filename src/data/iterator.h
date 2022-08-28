#if !defined(ITERATOR_H)
#define ITERATOR_H

#include <stdio.h>

struct D_Iterator;
struct Evaluator;

struct D_Iterator* iterator_new(struct Any* state, enum TypeId iteratorTypeId);
void iterator_free(struct D_Iterator* self);

bool iterator_boolValue(struct D_Iterator* self);
struct Any* iterator_getStateObject(struct D_Iterator* self);
bool iterator_hasNext(struct D_Iterator* self);
void iterator_markChildren(struct D_Iterator* self);
struct Any* iterator_next(struct D_Iterator* self);
void iterator_setStateObject(struct D_Iterator* self, struct Any* state);
void iterator_show(struct D_Iterator* self, FILE* fp);
size_t iterator_sizeOf(struct D_Iterator* self);
size_t iterator_structSize(void);
struct Any* iterator_typeOf(struct Any* self);

#endif
