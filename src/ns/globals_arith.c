#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "data/real.h"
#include "data/sequence.h"
#include "data/string.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/binop.h"
#include "expr/identifier.h"

void globals_operandException(char* operand, struct Any* lhs, struct Any* rhs, struct Evaluator* etor) {
    struct E_Identifier* dcolon = identifier_new("::");
    struct D_Symbol* lhsType = any_typeSymbol(lhs);
    struct D_Symbol* rhsType = any_typeSymbol(rhs);
    struct D_Array* args = array_newN(3,
                                      identifier_new(operand),
                                      binOp_new((struct Any*)lhs, dcolon, (struct Any*)lhsType),
                                      binOp_new((struct Any*)rhs, dcolon, (struct Any*)rhsType));
    evaluator_throwException(
        etor,
        symbol_new("Operator"),
        "argument types not supported",
        (struct Any*)args
    );
}

void globals_minus(struct Any* lhs, struct Any* rhs, struct Evaluator* etor) {
    enum TypeId typeIdLhs = any_typeId(lhs);
    if (typeIdLhs == T_Integer) {
        integer_operatorMinus((struct D_Integer*)lhs, rhs, etor);
    }
    else if (typeIdLhs == T_Real) {
        real_operatorMinus((struct D_Real*)lhs, rhs, etor);
    }
    else {
        globals_operandException("+", lhs, rhs, etor);
    }
}

void globals_percent(struct Any* lhs, struct Any* rhs, struct Evaluator* etor) {
    enum TypeId typeIdLhs = any_typeId(lhs);
    if (typeIdLhs == T_Integer) {
        integer_operatorPercent((struct D_Integer*)lhs, rhs, etor);
    }
    else if (typeIdLhs == T_Real) {
        real_operatorPercent((struct D_Real*)lhs, rhs, etor);
    }
    else if (typeIdLhs == T_Sequence) {
        sequence_operatorPercent((struct D_Sequence*)lhs, rhs, etor);
    }
    else {
        globals_operandException("+", lhs, rhs, etor);
    }
}

void globals_plus(struct Any* lhs, struct Any* rhs, struct Evaluator* etor) {
    enum TypeId typeIdLhs = any_typeId(lhs);
    if (typeIdLhs == T_Integer) {
        integer_operatorPlus((struct D_Integer*)lhs, rhs, etor);
    }
    else if (typeIdLhs == T_Real) {
        real_operatorPlus((struct D_Real*)lhs, rhs, etor);
    }
    else {
        globals_operandException("+", lhs, rhs, etor);
    }
}

void globals_slash(struct Any* lhs, struct Any* rhs, struct Evaluator* etor) {
    enum TypeId typeIdLhs = any_typeId(lhs);
    if (typeIdLhs == T_Integer) {
        integer_operatorSlash((struct D_Integer*)lhs, rhs, etor);
    }
    else if (typeIdLhs == T_Real) {
        real_operatorSlash((struct D_Real*)lhs, rhs, etor);
    }
    else {
        globals_operandException("+", lhs, rhs, etor);
    }
}

void globals_star(struct Any* lhs, struct Any* rhs, struct Evaluator* etor) {
    enum TypeId typeIdLhs = any_typeId(lhs);
    if (typeIdLhs == T_Integer) {
        integer_operatorStar((struct D_Integer*)lhs, rhs, etor);
    }
    else if (typeIdLhs == T_Real) {
        real_operatorStar((struct D_Real*)lhs, rhs, etor);
    }
    else {
        globals_operandException("+", lhs, rhs, etor);
    }
}
