#if !defined(NOTHING_H)
#define NOTHING_H

#include <stdio.h>

#include "main/typedefs.h"

struct D_Nothing;
struct Evaluator;

struct D_Nothing* nothing_new(void);
void nothing_free(struct D_Nothing* self);

int nothing_compare(struct D_Nothing* self, struct D_Nothing* other, struct Evaluator* etor);
HashCode nothing_hashCode(struct D_Nothing* self, struct Evaluator* etor);
void nothing_show(struct D_Nothing* self, FILE* fp);
size_t nothing_sizeOf(struct D_Nothing* self);
size_t nothing_structSize();

#endif
