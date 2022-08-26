#include "etor/evaluator.h"
#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "data/sequence.h"
#include "data/symbol.h"

void globals_sequence(struct Any* lhs, struct Any* rhs, struct Evaluator* etor) {
    enum TypeId typeIdLhs = any_typeId(lhs);
    enum TypeId typeIdRhs = any_typeId(rhs);
    if (typeIdLhs != typeIdRhs) {
        evaluator_throwException(
            etor,
            symbol_new("Sequence"),
            "not applicable to disparate argument types",
            (struct Any*)array_newN(2, lhs, rhs)
        );
    }
    if (typeIdLhs != T_Integer) {
        evaluator_throwException(
            etor,
            symbol_new("Sequence"),
            "argument types not supported",
            (struct Any*)array_newN(2, lhs, rhs)
        );
    }
    int from = integer_getValue((struct D_Integer*)lhs);
    int to = integer_getValue((struct D_Integer*)rhs);
    struct D_Sequence* seq = sequence_new(from, to, 1);
    evaluator_pushObj(etor, (struct Any*)seq);
}
