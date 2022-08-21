#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/binding.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/record.h"
#include "data/term.h"
#include "data/triple.h"
#include "data/tuple.h"
#include "dispatch/methodtable.h"
#include "expr/identifier.h"
#include "expr/recordspec.h"

static struct D_Triple* _error(struct Any* obj, struct Any* other, struct D_Triple* bindings) {
    (void)other;
    (void)bindings;
    fprintf(stderr, "ERROR: call to 'any_match' is not valid for type ID %d (%0x)\n", obj->typeId, obj->typeId);
    exit(1);
    return NULL;
}

static struct D_Triple* _isEqual(struct Any* obj, struct Any* other, struct D_Triple* bindings) {
    return any_isEqual(obj, other) ? bindings : NULL;
}

struct D_Triple* any_match(struct Any* obj, struct Any* other, struct D_Triple* bindings) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    if (methods != NULL) {
        struct D_Triple* (*method)(struct Any*, struct Any*, struct D_Triple*) = methods->m_match;
        if (method != NULL) {
            return method(obj, other, bindings);
        }
    }
    printf("%s no method to handle typeId %d %s\n", __func__, obj->typeId, TYPE_NAMES[obj->typeId]);
    switch (obj->typeId) {
        case T_NULL:
            return _error(obj, other, bindings);
        case T_Queue:
            return queue_match((struct D_Queue*)obj, other, bindings);
        case T_Abstraction:
            return _isEqual(obj, other, bindings);
        case T_Address:
            return _isEqual(obj, other, bindings);
        case T_Apply:
            return _isEqual(obj, other, bindings);
        case T_Array:
            return array_match((struct D_Array*)obj, other, bindings);
        case T_Binding:
            return binding_match((struct D_Binding*)obj, other, bindings);
        case T_BinOp:
            return _isEqual(obj, other, bindings);
        case T_Boolean:
            return _isEqual(obj, other, bindings);
        case T_BracketExpr:
            return _isEqual(obj, other, bindings);
        case T_Closure:
            return _isEqual(obj, other, bindings);
        case T_Cobegin:
            return _isEqual(obj, other, bindings);
        case T_Continuation:
            return _isEqual(obj, other, bindings);
        case T_Do:
            return _isEqual(obj, other, bindings);
        case T_Evaluator:
            return _isEqual(obj, other, bindings);
        case T_ExceptionHandler:
            return _isEqual(obj, other, bindings);
        case T_File:
            return _isEqual(obj, other, bindings);
        case T_HashTable:
            return _isEqual(obj, other, bindings);
        case T_Identifier:
            return identifier_match((struct E_Identifier*)obj, other, bindings);
        case T_If:
            return _isEqual(obj, other, bindings);
        case T_Integer:
            return _isEqual(obj, other, bindings);
        case T_Iterator:
            return _isEqual(obj, other, bindings);
        case T_Let:
            return _isEqual(obj, other, bindings);
        case T_LetIn:
            return _isEqual(obj, other, bindings);
        case T_LetRec:
            return _isEqual(obj, other, bindings);
        case T_List:
            return list_match((struct D_List*)obj, other, bindings);
        case T_Nil:
            return _isEqual(obj, other, bindings);
        case T_Primitive:
            return _isEqual(obj, other, bindings);
        case T_Protocol:
            return _isEqual(obj, other, bindings);
        case T_Real:
            return _isEqual(obj, other, bindings);
        case T_Record:
            return record_match((struct D_Record*)obj, other, bindings);
        case T_RecordDefinition:
            return _isEqual(obj, other, bindings);
        case T_RecordSpec:
            return recordSpec_match((struct E_RecordSpec*)obj, other, bindings);
        case T_REPL:
            return _isEqual(obj, other, bindings);
        case T_SavedEnv:
            return _isEqual(obj, other, bindings);
        case T_Sequence:
            return _isEqual(obj, other, bindings);
        case T_Set:
            return _isEqual(obj, other, bindings);
        case T_String:
            return _isEqual(obj, other, bindings);
        case T_StringBuffer:
            return _isEqual(obj, other, bindings);
        case T_StringStream:
            return _isEqual(obj, other, bindings);
        case T_Symbol:
            return _isEqual(obj, other, bindings);
        case T_Term:
            return term_match((struct D_Term*)obj, other, bindings);
        case T_Triple:
            return triple_match((struct D_Triple*)obj, other, bindings);
        case T_TryCatch:
            return _isEqual(obj, other, bindings);
        case T_Tuple:
            return tuple_match((struct D_Tuple*)obj, other, bindings);
        case T_Unsigned:
            return _isEqual(obj, other, bindings);
        case T_FINAL:
            return _error(obj, other, bindings);
    }
    return NULL;
}
