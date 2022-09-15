#include "etor/evaluator.h"
#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/symbol.h"
#include "expr/bracketexpr.h"
#include "expr/continuation.h"

static void _reassignBracketExpr(struct E_Continuation* contin, struct Evaluator* etor) {
    struct Any* arg = continuation_getArg(contin);
    struct Any* rhs = arg;
    struct Any* indexObj = evaluator_popObj(etor);
    struct Any* lhs = evaluator_popObj(etor);
    evaluator_pushObj(etor, (struct Any*)rhs);
    enum TypeId lhsTypeId = any_typeId(lhs);
    if (lhsTypeId == T_Array) {
        if (any_typeId(indexObj) == T_Integer) {
            int index = integer_getValue((struct D_Integer*)indexObj);
            array_set((struct D_Array*)lhs, index, rhs, etor);
        }
        else {
            evaluator_throwException(
                etor,
                symbol_new("reassign"),
                "illegal index type for array",
                (struct Any*)array_newN(2, indexObj, any_typeOf(indexObj))
            );
        }
    }
    else if (lhsTypeId == T_HashTable) {
        hashTable_put((struct D_HashTable*)lhs, indexObj, rhs, etor);
    }
    else {
        evaluator_throwException(
            etor,
            symbol_new("reassign"),
            "object is not index-assignable",
            (struct Any*)array_newN(3, lhs, any_typeOf(lhs), rhs)
        );
    }
}

void globals_reassign(struct Any* lhs, struct Any* rhs, struct Evaluator* etor) {
    enum TypeId typeIdLhs = any_typeId(lhs);
    if (typeIdLhs == T_Identifier) {
        evaluator_reassignBinding(etor, (struct E_Identifier*)lhs, rhs);
        evaluator_pushObj(etor, (struct Any*)rhs);
    }
    else if(typeIdLhs == T_BracketExpr) {
        struct E_BracketExpr* bracketExpr = (struct E_BracketExpr*)lhs;
        evaluator_pushExpr(etor, (struct Any*)continuation_new(_reassignBracketExpr, "reassignBracketExpr", rhs));
        evaluator_pushExpr(etor, bracketExpr_getIndex(bracketExpr));
        evaluator_pushExpr(etor, bracketExpr_getLhs(bracketExpr));
    }
    else {
        evaluator_throwException(
            etor,
            symbol_new("Reassign"),
            "left-hand side does not respond to operator ':='",
            (struct Any*)array_newN(3, lhs, any_typeOf(lhs), rhs)
        );
    }
}
