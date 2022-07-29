#include <stdio.h>
#include <stdlib.h>

#include "data/array.h"
#include "data/binding.h"
#include "data/hashtable.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/record.h"
#include "data/set.h"
#include "data/term.h"
#include "data/tuple.h"
#include "etor/evaluator.h"
#include "expr/abstraction.h"
#include "expr/apply.h"
#include "expr/binop.h"
#include "expr/bracketexpr.h"
#include "expr/cobegin.h"
#include "expr/continuation.h"
#include "expr/do.h"
#include "expr/exnhandler.h"
#include "expr/identifier.h"
#include "expr/if.h"
#include "expr/let.h"
#include "expr/letin.h"
#include "expr/letrec.h"
#include "expr/protocol.h"
#include "expr/recorddef.h"
#include "expr/recordspec.h"
#include "expr/savedenv.h"
#include "expr/trycatch.h"

static void _error(struct Any* obj, struct Evaluator* etor) {
    (void)etor;
    fprintf(stderr, "ERROR: call to 'any_eval' is not valid for type ID %d (%0x)\n", obj->typeId, obj->typeId);
    exit(1);
}

static void _self(struct Any* obj, struct Evaluator* etor) {
    evaluator_pushObj(etor, obj);
}

void any_eval(struct Any* obj, struct Evaluator* etor) {
    switch (obj->typeId) {
    case T_NULL:
        _error(obj, etor);
        break;
    case T_Abstraction:
        abstraction_eval((struct E_Abstraction*)obj, etor);
        break;
    case T_Address:
        _self(obj, etor);
        break;
    case T_Apply:
        apply_eval((struct E_Apply*)obj, etor);
        break;
    case T_Array:
        array_eval((struct D_Array*)obj, etor);
        break;
    case T_Binding:
        binding_eval((struct D_Binding*)obj, etor);
        break;
    case T_BinOp:
        binOp_eval((struct E_BinOp*)obj, etor);
        break;
    case T_Boolean:
        _self(obj, etor);
        break;
    case T_BracketExpr:
        bracketExpr_eval((struct E_BracketExpr*)obj, etor);
        break;
    case T_Closure:
        _self(obj, etor);
        break;
    case T_Cobegin:
        cobegin_eval((struct E_Cobegin*)obj, etor);
        break;
    case T_Continuation:
        continuation_eval((struct E_Continuation*)obj, etor);
        break;
    case T_Do:
        do_eval((struct E_Do*)obj, etor);
        break; 
    case T_Evaluator:
        _self(obj, etor);
        break;
    case T_ExceptionHandler:
        exceptionHandler_eval((struct E_ExceptionHandler*)obj, etor);
        break;
    case T_File:
        _self(obj, etor);
        break;
    case T_HashTable:
        hashTable_eval((struct D_HashTable*)obj, etor);
        break;
    case T_Identifier:
        identifier_eval((struct E_Identifier*)obj, etor);
        break;
    case T_Integer:
        _self(obj, etor);
        break;
    case T_If:
        if_eval((struct E_If*)obj, etor);
        break;
    case T_Let:
        let_eval((struct E_Let*)obj, etor);
        break;
    case T_LetIn:
        letIn_eval((struct E_LetIn*)obj, etor);
        break;
    case T_LetRec:
        letRec_eval((struct E_LetRec*)obj, etor);
        break;
    case T_List:
        list_eval((struct D_List*)obj, etor);
        break;
    case T_Nothing:
        _self(obj, etor);
        break;
    case T_Primitive:
        _self(obj, etor);
        break;
    case T_Protocol:
        protocol_eval((struct E_Protocol*)obj, etor);
        break;
    case T_Queue:
        queue_eval((struct D_Queue*)obj, etor);
        break;
    case T_Real:
        _self(obj, etor);
        break;
    case T_Record:
        _self(obj, etor);
        break;
    case T_RecordDefinition:
        recordDefinition_eval((struct E_RecordDefinition*)obj, etor);
        break;
    case T_RecordSpec:
        recordSpec_eval((struct E_RecordSpec*)obj, etor);
        break;
    case T_REPL:
        _self(obj, etor);
        break;
    case T_Set:
        set_eval((struct D_Set*)obj, etor);
        break;
    case T_SavedEnv:
        savedEnv_eval((struct E_SavedEnv*)obj, etor);
        break;
    case T_String:
        _self(obj, etor);
        break;
    case T_StringBuffer:
        _self(obj, etor);
        break;
    case T_StringStream:
        _self(obj, etor);
        break;
    case T_Symbol:
        _self(obj, etor);
        break;
    case T_Term:
        term_eval((struct D_Term*)obj, etor);
        break;
    case T_Tuple:
        tuple_eval((struct D_Tuple*)obj, etor);
        break;
    case T_Triple:
        _self(obj, etor);
        break;
    case T_TryCatch:
        tryCatch_eval((struct E_TryCatch*)obj, etor);
        break;
    case T_Unsigned:
        _self(obj, etor);
        break;
    case T_FINAL:
        _error(obj, etor);
        break;
    default:
        break;
    }
}
