#if !defined(QUOTE_H)
#define QUOTE_H

#include <stdbool.h>
#include <stdio.h>

#include "main/typedefs.h"  // for HashCode

struct Any;
struct D_Set;
struct D_Triple;
struct E_Quote;
struct Evaluator;

struct E_Quote* quote_new(struct Any* expr);
void quote_free(struct E_Quote* self);

int quote_compare(struct E_Quote* self, struct E_Quote* other, struct Evaluator* etor);
void quote_eval(struct E_Quote* self, struct Evaluator* etor);
void quote_freeVars(struct E_Quote* self, struct D_Set* freeVars,struct Evaluator* etor);
void quote_show(struct E_Quote* self, FILE* fd);
size_t quote_sizeOf(struct E_Quote* self);
size_t quote_structSize(void);

#endif
