#include "data/address.h"
#include "data/any.h"
#include "data/array.h"
#include "data/binding.h"
#include "data/boolean.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/nil.h"
#include "data/queue.h"
#include "data/real.h"
#include "data/sequence.h"
#include "data/string.h"
#include "data/symbol.h"
#include "data/unsigned.h"
#include "etor/evaluator.h"
#include "expr/bracketexpr.h"
#include "expr/identifier.h"
#include "data/symbol.h"

static int _fail(struct Any* obj, struct Any* other, struct Evaluator* etor) {
    evaluator_throwException(
        etor,
        symbol_new("Compare"),
        "unable to comare objects",
        (struct Any*)array_newN(2, obj, other)
    );
    return 0;
}

int any_compare(struct Any* obj, struct Any* other, struct Evaluator* etor) {
    switch (obj->typeId) {
        case T_NULL:
            return _fail(obj, other, etor);
        case T_Abstraction:
            return _fail(obj, other, etor);
        case T_Address:
            return address_compare((struct D_Address*)obj, (struct D_Address*)other, etor);
        case T_Apply:
            return _fail(obj, other, etor);
        case T_Array:
            return array_compare((struct D_Array*)obj, (struct D_Array*)other, etor);
        case T_Binding:
            return binding_compare((struct D_Binding*)obj, (struct D_Binding*)other, etor);
        case T_BinOp:
            return _fail(obj, other, etor);
        case T_Boolean:
            return boolean_compare((struct D_Boolean*)obj, (struct D_Boolean*)other, etor);
        case T_BracketExpr:
            return _fail(obj, other, etor);
        case T_Closure:
            return _fail(obj, other, etor);
        case T_Cobegin:
            return _fail(obj, other, etor);
        case T_Continuation:
            return _fail(obj, other, etor);
        case T_Do:
            return _fail(obj, other, etor);
        case T_Evaluator:
            return _fail(obj, other, etor);
        case T_ExceptionHandler:
            return _fail(obj, other, etor);
        case T_File:
            return _fail(obj, other, etor);
        case T_HashTable:
            return _fail(obj, other, etor);
        case T_Identifier:
            return identifier_compare((struct E_Identifier*)obj, (struct E_Identifier*)other, etor);
        case T_If:
            return _fail(obj, other, etor);
        case T_Integer:
            return integer_compare((struct D_Integer*)obj, (struct D_Integer*)other, etor);
        case T_Iterator:
            return _fail(obj, other, etor);
        case T_Let:
            return _fail(obj, other, etor);
        case T_LetIn:
            return _fail(obj, other, etor);
        case T_LetRec:
            return _fail(obj, other, etor);
        case T_List:
            return list_compare((struct D_List*)obj, (struct D_List*)other, etor);
        case T_Nil:
            return nil_compare((struct D_Nil*)obj, (struct D_Nil*)other, etor);
        case T_Primitive:
            return _fail(obj, other, etor);
        case T_Protocol:
            return _fail(obj, other, etor);
        case T_Queue:
            return queue_compare((struct D_Queue*)obj, (struct D_Queue*)other, etor);
        case T_Real:
            return real_compare((struct D_Real*)obj, (struct D_Real*)other, etor);
        case T_Record:
            return _fail(obj, other, etor);
        case T_RecordDefinition:
            return _fail(obj, other, etor);
        case T_RecordSpec:
            return _fail(obj, other, etor);
        case T_REPL:
            return _fail(obj, other, etor);
        case T_SavedEnv:
            return _fail(obj, other, etor);
        case T_Sequence:
            return sequence_compare((struct D_Sequence*)obj, (struct D_Sequence*)other, etor);
        case T_Set:
            return _fail(obj, other, etor);
        case T_String:
            return string_compare((struct D_String*)obj, (struct D_String*)other, etor);
        case T_StringBuffer:
            return _fail(obj, other, etor);
        case T_StringStream:
            return _fail(obj, other, etor);
        case T_Symbol:
            return symbol_compare((struct D_Symbol*)obj, (struct D_Symbol*)other, etor);
        case T_Term:
            return _fail(obj, other, etor);
        case T_Triple:
            return _fail(obj, other, etor);
        case T_TryCatch:
            return _fail(obj, other, etor);
        case T_Tuple:
            return _fail(obj, other, etor);
        case T_Unsigned:
            return unsigned_compare((struct D_Unsigned*)obj, (struct D_Unsigned*)other, etor);
        case T_FINAL:
        default:
            return _fail(obj, other, etor);
    }
}
