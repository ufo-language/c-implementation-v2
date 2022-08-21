#if !defined(ITERATOR_H)
#define ITERATOR_H

#include <stdio.h>

#include "main/typedefs.h"

struct D_Iterator;
struct Evaluator;

struct D_Iterator* iterator_new(void);
void iterator_free(struct D_Iterator* self);

bool iterator_boolValue(struct D_Iterator* self);
//int iterator_compare(struct D_Iterator* self, struct D_Iterator* other, struct Evaluator* etor);
//HashCode iterator_hashCode(struct D_Iterator* self, struct Evaluator* etor);
void iterator_markChildren(struct D_Iterator* self);
void iterator_show(struct D_Iterator* self, FILE* fp);
size_t iterator_sizeOf(struct D_Iterator* self);
size_t iterator_structSize(void);

#endif
