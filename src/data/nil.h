#if !defined(NIL_H)
#define NIL_H

#include <stdio.h>

#include "main/typedefs.h"  // for HashCode

struct D_Nil;
struct Evaluator;

struct D_Nil* nil_new(void);
void nil_free(struct D_Nil* self);

int nil_compare(struct D_Nil* self, struct D_Nil* other, struct Evaluator* etor);
bool nil_hashCode(struct D_Nil* self, HashCode* hashCode);
void nil_show(struct D_Nil* self, FILE* fp);
size_t nil_sizeOf(struct D_Nil* self);
size_t nil_structSize(void);

#endif
