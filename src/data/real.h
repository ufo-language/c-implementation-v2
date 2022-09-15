#if !defined(REAL_H)
#define REAL_H

#include <stdbool.h>
#include <stdio.h>

#include "any.h"

struct D_Real;
struct Evaluator;

struct D_Real* real_new(double value);
void real_free(struct D_Real* self);

bool real_boolValue(struct D_Real* self);
int real_compare(struct D_Real* self, struct D_Real* other, struct Evaluator* etor);
bool real_isEqual(struct D_Real* self, struct D_Real* other);
double real_getValue(struct D_Real* self);
void real_operatorMinus(struct D_Real* self, struct Any* rhs, struct Evaluator* etor);
void real_operatorPercent(struct D_Real* self, struct Any* rhs, struct Evaluator* etor);
void real_operatorPlus(struct D_Real* self, struct Any* rhs, struct Evaluator* etor);
void real_operatorSlash(struct D_Real* self, struct Any* rhs, struct Evaluator* etor);
void real_operatorStar(struct D_Real* self, struct Any* rhs, struct Evaluator* etor);
void real_show(struct D_Real* self, FILE* fp);
size_t real_sizeOf(struct D_Real* self);
size_t real_structSize(void);

#endif
