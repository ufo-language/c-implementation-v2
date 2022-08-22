#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data/any.h"
#include "data/closure.h"
#include "data/list.h"
#include "etor/evaluator.h"
#include "expr/binop.h"
#include "expr/continuation.h"
#include "expr/identifier.h"
#include "main/globals.h"
#include "gc/gc.h"

struct E_BinOp {
    struct Any obj;
    struct Any* lhs;
    struct E_Identifier* oper;
    struct Any* rhs;
    struct D_List* argList;
};

struct E_BinOp* binOp_new(struct Any* lhs, struct E_Identifier* oper, struct Any* rhs) {
    struct E_BinOp* self = (struct E_BinOp*)gc_alloc(T_BinOp);
    self->lhs = lhs;
    self->oper = oper;
    self->rhs = rhs;
    self->argList = list_new((struct Any*)lhs, (struct Any*)list_new((struct Any*)rhs, (struct Any*)EMPTY_LIST));
    return self;
}

void binOp_free(struct E_BinOp* self) {
    free(self);
}

void apply_contin(struct Evaluator* etor, struct Any* arg);

struct E_BinOp* binOp_deepCopy(struct E_BinOp* self) {
    return binOp_new(any_deepCopy(self->lhs), self->oper, any_deepCopy(self->rhs));
}

void binOp_eval(struct E_BinOp* self, struct Evaluator* etor) {
    struct E_Continuation* contin = continuation_new(apply_contin, "binOp", (struct Any*)self->argList);
    evaluator_pushExpr(etor, (struct Any*)contin);
    evaluator_pushExpr(etor, (struct Any*)self->oper);
}

void binOp_freeVars(struct E_BinOp* self, struct D_Set* freeVars, struct Evaluator* etor) {
    any_freeVars(self->lhs, freeVars, etor);
    any_freeVars((struct Any*)self->oper, freeVars, etor);
    any_freeVars(self->rhs, freeVars, etor);
}

void binOp_markChildren(struct E_BinOp* self) {
    any_mark((struct Any*)self->oper);
    any_mark((struct Any*)self->argList);
}

void binOp_show(struct E_BinOp* self, FILE* fp) {
    if (any_isA(self->lhs, T_BinOp)) {
        fputc('(', fp);
        any_show(self->lhs, fp);
        fputc(')', fp);
    }
    else {
        any_show(self->lhs, fp);
    }
    if (strcmp(":", identifier_getName(self->oper))) {
        fputc(' ', fp);
        identifier_show(self->oper, fp);
        fputc(' ', fp);
    }
    else {
        identifier_show(self->oper, fp);
    }
    if (any_isA(self->rhs, T_BinOp)) {
        fputc('(', fp);
        any_show(self->rhs, fp);
        fputc(')', fp);
    }
    else {
        any_show(self->rhs, fp);
    }
}

size_t binOp_sizeOf(struct E_BinOp* self) {
    return sizeof(self);
}

size_t binOp_structSize(void) {
    return sizeof(struct E_BinOp);
}
