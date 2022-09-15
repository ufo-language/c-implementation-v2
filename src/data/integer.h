#if !defined(INTEGER_H)
#define INTEGER_H

#include <stdbool.h>
#include <stdio.h>

#include "main/typedefs.h"  // for HashCode

struct D_Integer;
struct D_Iterator;
struct D_Stream;
struct Evaluator;

struct D_Integer* integer_new(int value);
void integer_initialize(struct D_Integer* self, int value);
void integer_free(struct D_Integer* self);

bool integer_boolValue(struct D_Integer* self);
int integer_compare(struct D_Integer* self, struct D_Integer* other, struct Evaluator* etor);
bool integer_isEqual(struct D_Integer* self, struct D_Integer* other);
int integer_getValue(struct D_Integer* self);
bool integer_hashCode(struct D_Integer* self, HashCode* hashCode);
struct D_Iterator* integer_iterator(struct D_Integer* self);
void integer_operatorMinus(struct D_Integer* self, struct Any* rhs, struct Evaluator* etor);
void integer_operatorPercent(struct D_Integer* self, struct Any* rhs, struct Evaluator* etor);
void integer_operatorPlus(struct D_Integer* self, struct Any* rhs, struct Evaluator* etor);
void integer_operatorSlash(struct D_Integer* self, struct Any* rhs, struct Evaluator* etor);
void integer_operatorStar(struct D_Integer* self, struct Any* rhs, struct Evaluator* etor);
void integer_show(struct D_Integer* self, FILE* fp);
size_t integer_sizeOf(struct D_Integer* self);
size_t integer_structSize(void);
void integer_writeToStream(struct D_Integer* self, struct D_Stream* stream);

#endif
