#if !defined(BOOLEAN_H)
#define BOOLEAN_H

#include <stdbool.h>
#include <stdio.h>

#include "main/typedefs.h"

struct D_Boolean;
struct Evaluator;

struct D_Boolean* boolean_from(bool value);

// Users should not call this function. Use boolean_from instead.
struct D_Boolean* boolean_new(bool value);
void boolean_free(struct D_Boolean* self);

bool boolean_boolValue(struct D_Boolean* self);
int boolean_compare(struct D_Boolean* self, struct D_Boolean* other, struct Evaluator* etor);
HashCode boolean_hashCode(struct D_Boolean* self, struct Evaluator* etor);
void boolean_show(struct D_Boolean* self, FILE* fp);
size_t boolean_sizeOf(struct D_Boolean* self);
size_t boolean_structSize();

#endif
