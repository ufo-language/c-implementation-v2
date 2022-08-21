#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/binding.h"
#include "data/hashtable.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/record.h"
#include "data/set.h"
#include "data/term.h"
#include "data/tuple.h"
#include "dispatch/methodtable.h"
#include "expr/abstraction.h"
#include "expr/apply.h"
#include "expr/binop.h"
#include "expr/bracketexpr.h"
#include "expr/cobegin.h"
#include "expr/do.h"
#include "expr/if.h"
#include "expr/let.h"
#include "expr/letin.h"
#include "expr/letrec.h"
#include "expr/trycatch.h"

static struct Any* _error(struct Any* obj) {
    fprintf(stderr, "ERROR: call to 'any_deepCopy' is not valid for type ID %d (%0x)\n", obj->typeId, obj->typeId);
    exit(1);
    return NULL;
}

struct Any* any_deepCopy(struct Any* obj) {
#if 1
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    if (methods != NULL) {
        struct Any* (*method)(struct Any*) = methods->m_deepCopy;
        if (method != NULL) {
            return method(obj);
        }
    }
    printf("%s no method to handle typeId %d %s\n", __func__, obj->typeId, TYPE_NAMES[obj->typeId]);
#endif
    switch (obj->typeId) {
        case T_NULL:
            return _error(obj);
        // simple case: return the object itself
        case T_Address:
        case T_Boolean:
        case T_File:
        case T_Identifier:
        case T_Integer:
        case T_Nil:
        case T_Primitive:
        case T_Real:
        case T_Sequence:
        case T_String:
        case T_Symbol:
        case T_Unsigned:
            return (struct Any*)obj;
        // the object is copyable
        case T_Abstraction:
            return (struct Any*)abstraction_deepCopy((struct E_Abstraction*)obj);
        case T_Apply:
            return (struct Any*)apply_deepCopy((struct E_Apply*)obj);
        case T_Array:
            return (struct Any*)array_deepCopy((struct D_Array*)obj);
        case T_Binding:
            return (struct Any*)binding_deepCopy((struct D_Binding*)obj);
        case T_BinOp:
            return (struct Any*)binOp_deepCopy((struct E_BinOp*)obj);
        case T_BracketExpr:
            return (struct Any*)bracketExpr_deepCopy((struct E_BracketExpr*)obj);
        case T_Cobegin:
            return (struct Any*)cobegin_deepCopy((struct E_Cobegin*)obj);
        case T_Do:
            return (struct Any*)do_deepCopy((struct E_Do*)obj);
        case T_HashTable:
            return (struct Any*)hashTable_deepCopy((struct D_HashTable*)obj);
        case T_If:
            return (struct Any*)if_deepCopy((struct E_If*)obj);
        case T_Let:
            return (struct Any*)let_deepCopy((struct E_Let*)obj);
        case T_LetIn:
            return (struct Any*)letIn_deepCopy((struct E_LetIn*)obj);
        case T_LetRec:
            return (struct Any*)letRec_deepCopy((struct E_LetRec*)obj);
        case T_List:
            return (struct Any*)list_deepCopy((struct D_List*)obj);
        case T_Queue:
            return (struct Any*)queue_deepCopy((struct D_Queue*)obj);
        case T_Record:
            return (struct Any*)record_deepCopy((struct D_Record*)obj);
        case T_Set:
            return (struct Any*)set_deepCopy((struct D_Set*)obj);
        case T_Term:
            return (struct Any*)term_deepCopy((struct D_Term*)obj);
        case T_TryCatch:
            return (struct Any*)tryCatch_deepCopy((struct E_TryCatch*)obj);
        case T_Tuple:
            return (struct Any*)tuple_deepCopy((struct D_Tuple*)obj);
        // the object is not copyable
        case T_Closure:
        case T_Continuation:
        case T_Evaluator:
        case T_ExceptionHandler:
        case T_Iterator:
        case T_Protocol:
        case T_RecordDefinition:
        case T_RecordSpec:
        case T_REPL:
        case T_SavedEnv:
        case T_StringBuffer:
        case T_StringStream:
        case T_Triple:
        case T_FINAL:
            return _error(obj);
        default:
            return 0;
    }
}

