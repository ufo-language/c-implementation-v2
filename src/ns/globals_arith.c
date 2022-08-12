#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "data/sequence.h"
#include "data/symbol.h"
#include "etor/evaluator.h"

void globals_minus(struct Any* lhs, struct Any* rhs, struct Evaluator* etor) {
    enum TypeId typeIdLhs = any_typeId(lhs);
    enum TypeId typeIdRhs = any_typeId(rhs);
    if (typeIdLhs == T_Integer && typeIdRhs == T_Integer) {
        int lhsInt = integer_getValue((struct D_Integer*)lhs);
        int rhsInt = integer_getValue((struct D_Integer*)rhs);
        struct D_Integer* modulus = integer_new(lhsInt - rhsInt);
        evaluator_pushObj(etor, (struct Any*)modulus);
        return;
    }
    evaluator_throwException(
        etor,
        symbol_new("Minus"),
        "argument types not supported",
        (struct Any*)array_newN(2, lhs, rhs)
    );
}

void globals_percent(struct Any* lhs, struct Any* rhs, struct Evaluator* etor) {
    enum TypeId typeIdLhs = any_typeId(lhs);
    enum TypeId typeIdRhs = any_typeId(rhs);
    if (typeIdLhs == T_Integer && typeIdRhs == T_Integer) {
        int lhsInt = integer_getValue((struct D_Integer*)lhs);
        int rhsInt = integer_getValue((struct D_Integer*)rhs);
        struct D_Integer* modulus = integer_new(lhsInt % rhsInt);
        evaluator_pushObj(etor, (struct Any*)modulus);
        return;
    }
    if (typeIdLhs == T_Sequence && typeIdRhs == T_Integer) {
        struct D_Sequence* seq = (struct D_Sequence*)lhs;
        int from = sequence_getFrom(seq);
        int to = sequence_getTo(seq);
        int by = integer_getValue((struct D_Integer*)rhs);
        struct D_Sequence* newSeq = sequence_new(from, to, by, etor);
        evaluator_pushObj(etor, (struct Any*)newSeq);
        return;
    }
    evaluator_throwException(
        etor,
        symbol_new("Percent"),
        "argument types not supported",
        (struct Any*)array_newN(2, lhs, rhs)
    );
}

void globals_plus(struct Any* lhs, struct Any* rhs, struct Evaluator* etor) {
    enum TypeId typeIdLhs = any_typeId(lhs);
    enum TypeId typeIdRhs = any_typeId(rhs);
    if (typeIdLhs == T_Integer && typeIdRhs == T_Integer) {
        int lhsInt = integer_getValue((struct D_Integer*)lhs);
        int rhsInt = integer_getValue((struct D_Integer*)rhs);
        struct D_Integer* modulus = integer_new(lhsInt + rhsInt);
        evaluator_pushObj(etor, (struct Any*)modulus);
        return;
    }
    evaluator_throwException(
        etor,
        symbol_new("Plus"),
        "argument types not supported",
        (struct Any*)array_newN(2, lhs, rhs)
    );
}

void globals_slash(struct Any* lhs, struct Any* rhs, struct Evaluator* etor) {
    enum TypeId typeIdLhs = any_typeId(lhs);
    enum TypeId typeIdRhs = any_typeId(rhs);
    if (typeIdLhs == T_Integer && typeIdRhs == T_Integer) {
        int lhsInt = integer_getValue((struct D_Integer*)lhs);
        int rhsInt = integer_getValue((struct D_Integer*)rhs);
        struct D_Integer* modulus = integer_new(lhsInt / rhsInt);
        evaluator_pushObj(etor, (struct Any*)modulus);
        return;
    }
    evaluator_throwException(
        etor,
        symbol_new("Slash"),
        "argument types not supported",
        (struct Any*)array_newN(2, lhs, rhs)
    );
}

void globals_star(struct Any* lhs, struct Any* rhs, struct Evaluator* etor) {
    enum TypeId typeIdLhs = any_typeId(lhs);
    enum TypeId typeIdRhs = any_typeId(rhs);
    if (typeIdLhs == T_Integer && typeIdRhs == T_Integer) {
        int lhsInt = integer_getValue((struct D_Integer*)lhs);
        int rhsInt = integer_getValue((struct D_Integer*)rhs);
        struct D_Integer* modulus = integer_new(lhsInt * rhsInt);
        evaluator_pushObj(etor, (struct Any*)modulus);
        return;
    }
    evaluator_throwException(
        etor,
        symbol_new("Star"),
        "argument types not supported",
        (struct Any*)array_newN(2, lhs, rhs)
    );
}
