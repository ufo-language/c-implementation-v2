#if !defined(INTEGER_H)
#define INTEGER_H

#include <stdbool.h>
#include <stdio.h>

#include "main/typedefs.h"  // for HashCode

struct D_Integer;
struct Evaluator;

struct D_Integer* integer_new(int value);
void integer_initialize(struct D_Integer* self, int value);
void integer_free(struct D_Integer* self);

bool integer_boolValue(struct D_Integer* self);
int integer_compare(struct D_Integer* self, struct D_Integer* other, struct Evaluator* etor);
bool integer_isEqual(struct D_Integer* self, struct D_Integer* other);
int integer_getValue(struct D_Integer* self);
bool integer_hashCode(struct D_Integer* self, HashCode* hashCode);
void integer_show(struct D_Integer* self, FILE* fp);
size_t integer_sizeOf(struct D_Integer* self);
size_t integer_structSize(void);

#endif
