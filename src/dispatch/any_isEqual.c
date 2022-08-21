#include <stdbool.h>
#include <stdlib.h>

#include "data/address.h"
#include "data/any.h"
#include "data/array.h"
#include "data/binding.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/real.h"
#include "data/record.h"
#include "data/sequence.h"
#include "data/set.h"
#include "data/string.h"
#include "data/triple.h"
#include "data/term.h"
#include "data/tuple.h"
#include "data/unsigned.h"
#include "dispatch/methodtable.h"

static bool _error(struct Any* obj, struct Any* other) {
    (void)other;
    fprintf(stderr, "ERROR: call to 'any_isEqual' is not valid for type ID %d (%0x)\n", obj->typeId, obj->typeId);
    exit(1);
    return false;
}

static bool _false(struct Any* obj, struct Any* other) {
    (void)obj;
    (void)other;
    return false;
}

bool any_isEqual(struct Any* obj, struct Any* other) {
    if (obj == other) {
        return true;
    }
    if (obj->typeId != other->typeId) {
        return false;
    }
    #if 1
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    if (methods != NULL) {
        bool (*method)(struct Any*, struct Any*) = methods->m_isEqual;
        if (method != NULL) {
            return method(obj, other);
        }
    }
    printf("%s no method to handle typeId %d %s\n", __func__, obj->typeId, TYPE_NAMES[obj->typeId]);
#endif
    switch (obj->typeId) {
        case T_NULL:
            return _error(obj, other);
        case T_Address:
            return address_isEqual((struct D_Address*)obj, (struct D_Address*)other);
        case T_Abstraction:
            return _false(obj, other);
        case T_Apply:
            return _false(obj, other);
        case T_Array:
            return array_isEqual((struct D_Array*)obj, (struct D_Array*)other);
        case T_Binding:
            return binding_isEqual((struct D_Binding*)obj, (struct D_Binding*)other);
        case T_BinOp:
            return _false(obj, other);
        case T_Boolean:
            return _false(obj, other);
        case T_BracketExpr:
            return _false(obj, other);
        case T_Closure:
            return _false(obj, other);
        case T_Continuation:
            return _false(obj, other);
        case T_Cobegin:
            return _false(obj, other);
        case T_Do:
            return _false(obj, other);
        case T_Evaluator:
            return _false(obj, other);
        case T_ExceptionHandler:
            return _false(obj, other);
        case T_File:
            return _false(obj, other);
        case T_HashTable:
            return hashTable_isEqual((struct D_HashTable*)obj, (struct D_HashTable*)other);
        case T_Identifier:
            return _false(obj, other);
        case T_If:
            return _false(obj, other);
        case T_Integer:
            return integer_isEqual((struct D_Integer*)obj, (struct D_Integer*)other);
        case T_Iterator:
            return _false(obj, other);
        case T_Let:
            return _false(obj, other);
        case T_LetIn:
            return _false(obj, other);
        case T_LetRec:
            return _false(obj, other);
        case T_List:
            return list_isEqual((struct D_List*)obj, (struct D_List*)other);
        case T_Nil:
            return _false(obj, other);
        case T_Primitive:
            return _false(obj, other);
        case T_Protocol:
            return _false(obj, other);
        case T_Queue:
            return queue_isEqual((struct D_Queue*)obj, (struct D_Queue*)other);
        case T_Real:
            return real_isEqual((struct D_Real*)obj, (struct D_Real*)other);
        case T_Record:
            return record_isEqual((struct D_Record*)obj, (struct D_Record*)other);
        case T_RecordDefinition:
            return _false(obj, other);
        case T_RecordSpec:
            return _false(obj, other);
        case T_REPL:
            return _false(obj, other);
        case T_SavedEnv:
            return _false(obj, other);
        case T_Sequence:
            return sequence_isEqual((struct D_Sequence*)obj, (struct D_Sequence*)other);
        case T_Set:
            return set_isEqual((struct D_Set*)obj, (struct D_Set*)other);
        case T_String:
            return string_isEqual((struct D_String*)obj, (struct D_String*)other);
        case T_StringBuffer:
            return _false(obj, other);
        case T_StringStream:
            return _false(obj, other);
        case T_Symbol:
            return _false(obj, other);
        case T_Term:
            return term_isEqual((struct D_Term*)obj, (struct D_Term*)other);
        case T_Triple:
            return triple_isEqual((struct D_Triple*)obj, (struct D_Triple*)other);
        case T_TryCatch:
            return _false(obj, other);
        case T_Tuple:
            return tuple_isEqual((struct D_Tuple*)obj, (struct D_Tuple*)other);
        case T_Unsigned:
            return unsigned_isEqual((struct D_Unsigned*)obj, (struct D_Unsigned*)other);
        case T_FINAL:
            return _error(obj, other);
        default:
            return false;
    }
}
