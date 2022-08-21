#include <stdio.h>
#include <stdlib.h>

#include "data/address.h"
#include "data/any.h"
#include "data/array.h"
#include "data/boolean.h"
#include "data/file.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/iterator.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/real.h"
#include "data/sequence.h"
#include "data/set.h"
#include "data/string.h"
#include "data/stringbuffer.h"
#include "data/stringstream.h"
#include "data/symbol.h"
#include "data/term.h"
#include "data/unsigned.h"

static bool _error(struct Any* obj) {
    fprintf(stderr, "ERROR: call to 'any_boolValue' is not valid for type ID %d (%0x)\n", obj->typeId, obj->typeId);
    exit(1);
    return false;
}

static bool _false(struct Any* obj) {
    (void)obj;
    return false;
}

static bool _true(struct Any* obj) {
    (void)obj;
    return true;
}

bool any_boolValue(struct Any* obj) {
    switch (obj->typeId) {
        case T_NULL:
            return _error(obj);
        case T_Abstraction:
            return _true(obj);
        case T_Address:
            return address_boolValue((struct D_Address*)obj);
        case T_Apply:
            return _true(obj);
        case T_Array:
            return 0 != array_count((struct D_Array*)obj);
        case T_Binding:
            return _true(obj);
        case T_BinOp:
            return _true(obj);
        case T_Boolean:
            return boolean_boolValue((struct D_Boolean*)obj);
        case T_BracketExpr:
            return _true(obj);
        case T_Closure:
            return _true(obj);
        case T_Cobegin:
            return _true(obj);
        case T_Continuation:
            return _true(obj);
        case T_Do:
            return _true(obj);
        case T_Evaluator:
            return _true(obj);
        case T_ExceptionHandler:
            return _true(obj);
        case T_File:
            return file_boolValue((struct D_File*)obj);
        case T_HashTable:
            return hashTable_boolValue((struct D_HashTable*)obj);
        case T_Integer:
            return integer_boolValue((struct D_Integer*)obj);
        case T_Identifier:
            return _true(obj);
        case T_If:
            return _true(obj);
        case T_Iterator:
            return iterator_boolValue((struct D_Iterator*)obj);
        case T_Let:
            return _true(obj);
        case T_LetIn:
            return _true(obj);
        case T_LetRec:
            return _true(obj);
        case T_List:
            return list_boolValue((struct D_List*)obj);
        case T_Nil:
            return _false(obj);
        case T_Primitive:
            return _true(obj);
        case T_Protocol:
            return _true(obj);
        case T_Queue:
            return queue_boolValue((struct D_Queue*)obj);
        case T_Real:
            return real_boolValue((struct D_Real*)obj);
        case T_Record:
            return _true(obj);
        case T_RecordDefinition:
            return _true(obj);
        case T_RecordSpec:
            return _true(obj);
        case T_REPL:
            return _true(obj);
        case T_SavedEnv:
            return _true(obj);
        case T_Sequence:
            return sequence_boolValue((struct D_Sequence*)obj);
        case T_Set:
            return set_boolValue((struct D_Set*)obj);
        case T_String:
            return string_boolValue((struct D_String*)obj);
        case T_StringBuffer:
            return stringBuffer_boolValue((struct D_StringBuffer*)obj);
        case T_StringStream:
            return stringStream_boolValue((struct D_StringStream*)obj);
        case T_Symbol:
            return _true(obj);
        case T_Term:
            return term_boolValue((struct D_Term*)obj);
        case T_Triple:
            return _true(obj);
        case T_TryCatch:
            return _true(obj);
        case T_Tuple:
            return _true(obj);
        case T_Unsigned:
            return unsigned_boolValue((struct D_Unsigned*)obj);
        case T_FINAL:
            return _error(obj);
        default:
            return false;
    }
}
