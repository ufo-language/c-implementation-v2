#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "data/symbol.h"
#include "etor/evaluator.h"

void globals_minus(struct Any* lhs, struct Any* rhs, struct Evaluator* etor) {
    enum TypeId typeIdLhs = any_typeId(lhs);
    if (typeIdLhs == T_Integer) {
        enum TypeId typeIdRhs = any_typeId(rhs);
        if (typeIdRhs == T_Integer) {
            int sum = integer_getValue((struct D_Integer*)lhs) - integer_getValue((struct D_Integer*)rhs);
            evaluator_pushObj(etor, (struct Any*)integer_new(sum));
        }
    }
    else {
        evaluator_throwException(
            etor,
            symbol_new("Add"),
            "left-hand side does not respond to operator '-'",
            (struct Any*)array_newN(2, lhs, rhs)
        );
    }
}

void globals_plus(struct Any* lhs, struct Any* rhs, struct Evaluator* etor) {
    enum TypeId typeIdLhs = any_typeId(lhs);
    if (typeIdLhs == T_Integer) {
        enum TypeId typeIdRhs = any_typeId(rhs);
        if (typeIdRhs == T_Integer) {
            int sum = integer_getValue((struct D_Integer*)lhs) + integer_getValue((struct D_Integer*)rhs);
            evaluator_pushObj(etor, (struct Any*)integer_new(sum));
        }
    }
    else {
        evaluator_throwException(
            etor,
            symbol_new("Add"),
            "left-hand side does not respond to operator '+'",
            (struct Any*)array_newN(2, lhs, rhs)
        );
    }
}

void globals_slash(struct Any* lhs, struct Any* rhs, struct Evaluator* etor) {
    enum TypeId typeIdLhs = any_typeId(lhs);
    if (typeIdLhs == T_Integer) {
        enum TypeId typeIdRhs = any_typeId(rhs);
        if (typeIdRhs == T_Integer) {
            int sum = integer_getValue((struct D_Integer*)lhs) / integer_getValue((struct D_Integer*)rhs);
            evaluator_pushObj(etor, (struct Any*)integer_new(sum));
        }
    }
    else {
        evaluator_throwException(
            etor,
            symbol_new("Add"),
            "left-hand side does not respond to operator '-'",
            (struct Any*)array_newN(2, lhs, rhs)
        );
    }
}

void globals_star(struct Any* lhs, struct Any* rhs, struct Evaluator* etor) {
    enum TypeId typeIdLhs = any_typeId(lhs);
    if (typeIdLhs == T_Integer) {
        enum TypeId typeIdRhs = any_typeId(rhs);
        if (typeIdRhs == T_Integer) {
            int sum = integer_getValue((struct D_Integer*)lhs) * integer_getValue((struct D_Integer*)rhs);
            evaluator_pushObj(etor, (struct Any*)integer_new(sum));
        }
    }
    else {
        evaluator_throwException(
            etor,
            symbol_new("Add"),
            "left-hand side does not respond to operator '-'",
            (struct Any*)array_newN(2, lhs, rhs)
        );
    }
}
