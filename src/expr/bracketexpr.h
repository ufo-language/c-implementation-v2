#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "gc/gc.h"
#include "main/globals.h"

struct D_Set;
struct E_BracketExpr;

struct E_BracketExpr* bracketExpr_new(struct Any* lhs, struct Any* index);
void bracketExpr_free(struct E_BracketExpr* self);

struct E_BracketExpr* bracketExpr_deepCopy(struct E_BracketExpr* self);
void bracketExpr_eval(struct E_BracketExpr* self, struct Evaluator* etor);
void bracketExpr_freeVars(struct E_BracketExpr* self, struct D_Set* freeVars, struct Evaluator* etor);
struct Any* bracketExpr_getIndex(struct E_BracketExpr* self);
struct Any* bracketExpr_getLhs(struct E_BracketExpr* self);
void bracketExpr_markChildren(struct E_BracketExpr* self);
void bracketExpr_show(struct E_BracketExpr* self, FILE* fp);
size_t bracketExpr_sizeOf(struct E_BracketExpr* self);
size_t bracketExpr_structSize(void);
