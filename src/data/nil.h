#if !defined(NIL_H)
#define NIL_H

#include <stdio.h>

#include "main/typedefs.h"

struct D_Nil;
struct Evaluator;

struct D_Nil* nil_new(void);
void nil_free(struct D_Nil* self);

int nil_compare(struct D_Nil* self, struct D_Nil* other, struct Evaluator* etor);
HashCode nil_hashCode(struct D_Nil* self, struct Evaluator* etor);
void nil_show(struct D_Nil* self, FILE* fp);
size_t nil_sizeOf(struct D_Nil* self);
size_t nil_structSize();

#endif
