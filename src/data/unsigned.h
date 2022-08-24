#if !defined(UNSIGNED_H)
#define UNSIGNED_H

#include <stdbool.h>
#include <stdio.h>

#include "main/typedefs.h"  // for HashCode

struct D_Unsigned;
struct Evaluator;

struct D_Unsigned* unsigned_new(size_t value);
void unsigned_initialize(struct D_Unsigned* self, size_t value);
void unsigned_free(struct D_Unsigned* self);

bool unsigned_boolValue(struct D_Unsigned* self);
int unsigned_compare(struct D_Unsigned* self, struct D_Unsigned* other, struct Evaluator* etor);
void unsigned_display(struct D_Unsigned* self, FILE* fp);
bool unsigned_isEqual(struct D_Unsigned* self, struct D_Unsigned* other);
size_t unsigned_getValue(struct D_Unsigned* self);
bool unsigned_hashCode(struct D_Unsigned* self, HashCode* hashCode);
void unsigned_show(struct D_Unsigned* self, FILE* fp);
size_t unsigned_sizeOf(struct D_Unsigned* self);
size_t unsigned_structSize(void);

#endif
