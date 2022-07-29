#if !defined(BINOP_H)
#define BINOP_H

#include <stdio.h>

struct Any;
struct D_List;
struct D_Set;
struct E_BinOp;
struct E_Identifier;
struct Evaluator;

struct E_BinOp* binOp_new(struct Any* lhs, struct E_Identifier* oper, struct Any* rhs);
void binOp_free(struct E_BinOp* self);

struct E_BinOp* binOp_deepCopy(struct E_BinOp* self);
void binOp_eval(struct E_BinOp* self, struct Evaluator* etor);
void binOp_freeVars(struct E_BinOp* self, struct D_Set* freeVars, struct Evaluator* etor);
void binOp_markChildren(struct E_BinOp* self);
void binOp_show(struct E_BinOp* self, FILE* fp);
size_t binOp_sizeOf(struct E_BinOp* self);
size_t binOp_structSize();

#endif
