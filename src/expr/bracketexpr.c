#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "expr/do.h"
#include "gc/gc.h"
#include "main/globals.h"

struct E_BracketExpr {
    struct Any obj;
    struct Any* lhs;
    struct Any* index;
};

struct E_BracketExpr* bracketExpr_new(struct Any* lhs, struct Any* index) {
    struct E_BracketExpr* self = (struct E_BracketExpr*)gc_alloc(T_BracketExpr);
    self->lhs = lhs;
    self->index = index;
    return self;
}

void bracketExpr_free(struct E_BracketExpr* self) {
    free(self);
}

void ns_array_get(struct Evaluator* etor, struct D_List* args);
void ns_hash_get(struct Evaluator* etor, struct D_List* args);
void ns_list_get(struct Evaluator* etor, struct D_List* args);
void ns_set_has(struct Evaluator* etor, struct D_List* args);
void ns_string_get(struct Evaluator* etor, struct D_List* args);

static void _evalContin(struct Evaluator* etor, struct Any* arg) {
    (void)arg;
    struct Any* indexObj = evaluator_popObj(etor);
    struct Any* lhs = evaluator_popObj(etor);
    enum TypeId lhsType = any_typeId(lhs);
    struct D_List* args = list_from(lhs, indexObj, 0);
    if (lhsType == T_Array) {
        ns_array_get(etor, args);
    }
    else if (lhsType == T_HashTable) {
        ns_hash_get(etor, args);
    }
    else if (lhsType == T_Set) {
        ns_set_has(etor, args);
    }
    else if (lhsType == T_String) {
        ns_string_get(etor, args);
    }
    else if (lhsType == T_List) {
        ns_list_get(etor, args);
    }
    else {
        evaluator_throwException(
            etor,
            symbol_new("Index"),
            "object is not indexable",
            lhs
        );
    }
}

struct E_BracketExpr* bracketExpr_deepCopy(struct E_BracketExpr* self) {
    return bracketExpr_new(any_deepCopy(self->lhs), any_deepCopy(self->index));
}

void bracketExpr_eval(struct E_BracketExpr* self, struct Evaluator* etor) {
    struct E_Continuation* contin = continuation_new(_evalContin, "BracketExpr", (struct Any*)NIL);
    evaluator_pushExpr(etor, (struct Any*)contin);
    evaluator_pushExpr(etor, self->index);
    evaluator_pushExpr(etor, self->lhs);
}

void bracketExpr_freeVars(struct E_BracketExpr* self, struct D_Set* freeVars, struct Evaluator* etor) {
    any_freeVars(self->lhs, freeVars, etor);
    any_freeVars(self->index, freeVars, etor);
}

struct Any* bracketExpr_getIndex(struct E_BracketExpr* self) {
    return self->index;
}

struct Any* bracketExpr_getLhs(struct E_BracketExpr* self) {
    return self->lhs;
}

void bracketExpr_markChildren(struct E_BracketExpr* self) {
    any_mark((struct Any*)self->lhs);
    any_mark((struct Any*)self->index);
}

void bracketExpr_show(struct E_BracketExpr* self, FILE* fp) {
    any_show(self->lhs, fp);
    fputc('[', fp);
    any_show(self->index, fp);
    fputc(']', fp);
}

size_t bracketExpr_sizeOf(struct E_BracketExpr* self) {
    return sizeof(self);
}

size_t bracketExpr_structSize() {
    return sizeof(struct E_BracketExpr);
}
