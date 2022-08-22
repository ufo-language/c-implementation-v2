#if !defined(TRYCATCH_H)
#define TRYCATCH_H

#include <stdio.h>

struct Any;
struct D_Array;
struct D_Set;
struct E_Abstraction;
struct E_TryCatch;
struct Evaluator;

struct E_TryCatch* tryCatch_new(struct Any* tryExpr, struct E_Abstraction* catchExpr);
void tryCatch_free(struct E_TryCatch* self);

struct E_TryCatch* tryCatch_deepCopy(struct E_TryCatch* self);
void tryCatch_eval(struct E_TryCatch* self, struct Evaluator* etor);
void tryCatch_freeVars(struct E_TryCatch* self, struct D_Set* freeVars, struct Evaluator* etor);
void tryCatch_markChildren(struct E_TryCatch* self);
void tryCatch_show(struct E_TryCatch* self, FILE* fp);
size_t tryCatch_sizeOf(struct E_TryCatch* self);
size_t tryCatch_structSize(void);

#endif
