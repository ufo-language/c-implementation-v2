#include <stdio.h>
#include <stdlib.h>

#include "data/address.h"
#include "data/array.h"
#include "data/binding.h"
#include "data/boolean.h"
#include "data/integer.h"
#include "data/nil.h"
#include "data/primitive.h"
#include "data/sequence.h"
#include "data/string.h"
#include "data/symbol.h"
#include "data/tuple.h"
#include "data/unsigned.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"

static HashCode _error(struct Any* obj, struct Evaluator* etor) {
    (void)etor;
    fprintf(stderr, "ERROR: call to 'any_eval' is not valid for type ID %d (%0x)\n", obj->typeId, obj->typeId);
    exit(1);
    return 0;
}

static HashCode _noHash(struct Any* obj, struct Evaluator* etor) {
    evaluator_throwException(
        etor,
        symbol_new("HashCode"),
        "object is not hashable",
        obj
    );
    return 0;
}

HashCode any_hashCode(struct Any* obj, struct Evaluator* etor) {
    switch (obj->typeId) {
        case T_NULL:
            return _error(obj, etor);
        case T_Address:
            return address_hashCode((struct D_Address*)obj, etor);
        case T_Abstraction:
            return _noHash(obj, etor);
        case T_Apply:
            return _noHash(obj, etor);
        case T_Array:
            return array_hashCode((struct D_Array*)obj, etor);
        case T_Binding:
            return binding_hashCode((struct D_Binding*)obj, etor);
        case T_BinOp:
            return _noHash(obj, etor);
        case T_Boolean:
            return boolean_hashCode((struct D_Boolean*)obj, etor);
        case T_BracketExpr:
            return _noHash(obj, etor);
        case T_Closure:
            return _noHash(obj, etor);
        case T_Continuation:
            return _noHash(obj, etor);
        case T_Cobegin:
            return _noHash(obj, etor);
        case T_Do:
            return _noHash(obj, etor);
        case T_Evaluator:
            return _noHash(obj, etor);
        case T_ExceptionHandler:
            return _noHash(obj, etor);
        case T_File:
            return _noHash(obj, etor);
        case T_HashTable:
            return _noHash(obj, etor);
        case T_Identifier:
            return identifier_hashCode((struct E_Identifier*)obj, etor);
        case T_If:
            return _noHash(obj, etor);
        case T_Integer:
            return integer_hashCode((struct D_Integer*)obj, etor);
        case T_Iterator:
            return _noHash(obj, etor);
        case T_Let:
            return _noHash(obj, etor);
        case T_LetIn:
            return _noHash(obj, etor);
        case T_LetRec:
            return _noHash(obj, etor);
        case T_List:
            return _noHash(obj, etor);
        case T_Nil:
            return nil_hashCode((struct D_Nil*)obj, etor);
        case T_Primitive:
            return primitive_hashCode((struct D_Primitive*)obj, etor);
        case T_Protocol:
            return _noHash(obj, etor);
        case T_Queue:
            return _noHash(obj, etor);
        case T_Real:
            return _noHash(obj, etor);
        case T_Record:
            return _noHash(obj, etor);
        case T_RecordSpec:
            return _noHash(obj, etor);
        case T_RecordDefinition:
            return _noHash(obj, etor);
        case T_REPL:
            return _noHash(obj, etor);
        case T_SavedEnv:
            return _noHash(obj, etor);
        case T_Sequence:
            return sequence_hashCode((struct D_Sequence*)obj, etor);
        case T_Set:
            return _noHash(obj, etor);
        case T_String:
            return string_hashCode((struct D_String*)obj, etor);
        case T_Symbol:
            return symbol_hashCode((struct D_Symbol*)obj, etor);
        case T_StringBuffer:
            return _noHash(obj, etor);
        case T_StringStream:
            return _noHash(obj, etor);
        case T_Term:
            return _noHash(obj, etor);
        case T_Triple:
            return _noHash(obj, etor);
        case T_TryCatch:
            return _noHash(obj, etor);
        case T_Tuple:
            return tuple_hashCode((struct D_Tuple*)obj, etor);
        case T_Unsigned:
            return unsigned_hashCode((struct D_Unsigned*)obj, etor);
        case T_FINAL: 
            return _error(obj, etor);
        default:
           return 0;
    }
}
