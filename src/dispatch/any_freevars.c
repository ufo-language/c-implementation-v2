#include <stdio.h>
#include <stdlib.h>

#include "data/array.h"
#include "data/binding.h"
#include "data/closure.h"
#include "data/hashtable.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/record.h"
#include "data/set.h"
#include "data/term.h"
#include "data/triple.h"
#include "data/tuple.h"
#include "dispatch/methodtable.h"
#include "etor/evaluator.h"
#include "expr/apply.h"
#include "expr/abstraction.h"
#include "expr/binop.h"
#include "expr/bracketexpr.h"
#include "expr/cobegin.h"
#include "expr/do.h"
#include "expr/identifier.h"
#include "expr/if.h"
#include "expr/let.h"
#include "expr/letin.h"
#include "expr/letrec.h"
#include "expr/protocol.h"
#include "expr/recorddef.h"
#include "expr/recordspec.h"
#include "expr/trycatch.h"

static void _error(struct Any* obj, struct D_Set* freeVars, struct Evaluator* etor) {
    (void)freeVars;
    (void)etor;
    fprintf(stderr, "ERROR: call to 'any_freeVars' is not valid for type ID %d (%0x)\n", obj->typeId, obj->typeId);
    exit(1);
}

static void _simpleMatch(struct Any* obj, struct D_Set* freeVars, struct Evaluator* etor) {
    (void)obj;
    (void)freeVars;
    (void)etor;
}

void any_freeVars(struct Any* obj, struct D_Set* freeVars, struct Evaluator* etor) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    if (methods != NULL) {
        void (*method)(struct Any*, struct D_Set*, struct Evaluator*) = methods->m_freeVars;
        if (method != NULL) {
            method(obj, freeVars, etor);
            return;
        }
    }
    printf("%s no method to handle typeId %d %s\n", __func__, obj->typeId, TYPE_NAMES[obj->typeId]);
    switch (obj->typeId) {
        case T_NULL:
            _error(obj, freeVars, etor);
            break;
        case T_Abstraction:
            abstraction_freeVars((struct E_Abstraction*)obj, freeVars, etor);
            break;
        case T_Address:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_Apply:
            apply_freeVars((struct E_Apply*)obj, freeVars, etor);
            break;
        case T_Array:
            array_freeVars((struct D_Array*)obj, freeVars, etor);
            break;
        case T_Binding:
            binding_freeVars((struct D_Binding*)obj, freeVars, etor);
            break;
        case T_BinOp:
            binOp_freeVars((struct E_BinOp*)obj, freeVars, etor);
            break;
        case T_Boolean:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_BracketExpr:
            bracketExpr_freeVars((struct E_BracketExpr*)obj, freeVars, etor);
            break;
        case T_Closure:
            closure_freeVars((struct D_Closure*)obj, freeVars, etor);
            break;
        case T_Cobegin:
            cobegin_freeVars((struct E_Cobegin*)obj, freeVars, etor);
            break;
        case T_Continuation:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_Do:
            do_freeVars((struct E_Do*)obj, freeVars, etor);
            break;
        case T_Evaluator:
            _simpleMatch(obj, freeVars, etor);
            break; 
        case T_ExceptionHandler:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_File:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_HashTable:
            hashTable_freeVars((struct D_HashTable*)obj, freeVars, etor);
            break;
        case T_Identifier:
            identifier_freeVars((struct E_Identifier*)obj, freeVars, etor);
            break;
        case T_If:
            if_freeVars((struct E_If*)obj, freeVars, etor);
            break;
        case T_Integer:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_Iterator:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_Let:
            let_freeVars((struct E_Let*)obj, freeVars, etor);
            break;
        case T_LetIn:
            letIn_freeVars((struct E_LetIn*)obj, freeVars, etor);
            break;
        case T_LetRec:
            letRec_freeVars((struct E_LetRec*)obj, freeVars, etor);
            break;
        case T_List:
            list_freeVars((struct D_List*)obj, freeVars, etor);
            break;
        case T_Nil:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_Primitive:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_Protocol:
            protocol_freeVars((struct E_Protocol*)obj, freeVars, etor);
            break;
        case T_Queue:
            queue_freeVars((struct D_Queue*)obj, freeVars, etor);
            break;
        case T_Real:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_Record:
            record_freeVars((struct D_Record*)obj, freeVars, etor);
            break;
        case T_RecordDefinition:
            recordDefinition_freeVars((struct E_RecordDefinition*)obj, freeVars, etor);
            break;
        case T_RecordSpec:
            recordSpec_freeVars((struct E_RecordSpec*)obj, freeVars, etor);
            break;
        case T_REPL:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_SavedEnv:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_Sequence:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_Set:
            set_freeVars((struct D_Set*)obj, freeVars, etor);
            break;
        case T_String:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_StringBuffer:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_StringStream:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_Symbol:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_Term:
            term_freeVars((struct D_Term*)obj, freeVars, etor);
            break;
        case T_Triple:
            triple_freeVars((struct D_Triple*)obj, freeVars, etor);
            break;
        case T_Tuple:
            tuple_freeVars((struct D_Tuple*)obj, freeVars, etor);
            break;
        case T_TryCatch:
            tryCatch_freeVars((struct E_TryCatch*)obj, freeVars, etor);
            break;
        case T_Unsigned:
            _simpleMatch(obj, freeVars, etor);
            break;
        case T_FINAL:
            _error(obj, freeVars, etor);
            break;
        default:
            break;
    }
}
