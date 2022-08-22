#include <assert.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/binding.h"
#include "data/closure.h"
#include "data/file.h"
#include "data/hashtable.h"
#include "data/iterator.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/record.h"
#include "data/set.h"
#include "data/term.h"
#include "data/triple.h"
#include "data/tuple.h"
#include "dispatch/methodtable.h"
#include "etor/evaluator.h"
#include "expr/abstraction.h"
#include "expr/apply.h"
#include "expr/binop.h"
#include "expr/bracketexpr.h"
#include "expr/cobegin.h"
#include "expr/continuation.h"
#include "expr/do.h"
#include "expr/exnhandler.h"
#include "expr/if.h"
#include "expr/let.h"
#include "expr/letin.h"
#include "expr/letrec.h"
#include "expr/protocol.h"
#include "expr/recorddef.h"
#include "expr/recordspec.h"
#include "expr/savedenv.h"
#include "expr/trycatch.h"
#include "repl/repl.h"

static void _error(struct Any* obj) {
    fprintf(stderr, "ERROR: call to 'any_mark' is not valid for type ID %d (%0x)\n", obj->typeId, obj->typeId);
    exit(1);
}

static void _mark(struct Any* obj) {
    (void)obj;
}

void any_mark(struct Any* obj) {
    assert(obj != NULL);
    if (obj->isMarked) {
        return;
    }
    obj->isMarked = true;
#if 1
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    if (methods != NULL) {
        void (*method)(struct Any*) = methods->m_markChildren;
        if (method != NULL) {
            method(obj);
            return;
        }
    }
    printf("%s no method to handle typeId %d %s\n", __func__, obj->typeId, TYPE_NAMES[obj->typeId]);
#endif
    switch (obj->typeId) {
        case T_NULL:
            _error(obj);
            break;
        case T_Abstraction:
            abstraction_markChildren((struct E_Abstraction*)obj);
            break;
        case T_Address:
            _mark(obj);
            break;
        case T_Apply:
            apply_markChildren((struct E_Apply*)obj);
            break;
        case T_Array:
            array_markChildren((struct D_Array*)obj);
            break;
        case T_Binding:
            binding_markChildren((struct D_Binding*)obj);
            break;
        case T_BinOp:
            binOp_markChildren((struct E_BinOp*)obj);
            break;
        case T_Boolean:
            _mark(obj);
            break;
        case T_BracketExpr:
            bracketExpr_markChildren((struct E_BracketExpr*)obj);
            break;
        case T_Closure:
            closure_markChildren((struct D_Closure*)obj);
            break;
        case T_Cobegin:
            cobegin_markChildren((struct E_Cobegin*)obj);
            break;
        case T_Continuation:
            continuation_markChildren((struct E_Continuation*)obj);
            break;
        case T_Do:
            do_markChildren((struct E_Do*)obj);
            break;
        case T_Evaluator:
            evaluator_markChildren((struct Evaluator*)obj);
            break;
        case T_ExceptionHandler:
            exceptionHandler_markChildren((struct E_ExceptionHandler*)obj);
            break;
        case T_File:
            file_markChildren((struct D_File*)obj);
            break;
        case T_HashTable:
            hashTable_markChildren((struct D_HashTable*)obj);
            break;
        case T_Identifier:
            _mark(obj);
            break;
        case T_If:
            if_markChildren((struct E_If*)obj);
            break;
        case T_Integer:
            _mark(obj);
            break;
        case T_Iterator:
            iterator_markChildren((struct D_Iterator*)obj);
            break;
        case T_Let:
            let_markChildren((struct E_Let*)obj);
            break;
        case T_LetIn:
            letIn_markChildren((struct E_LetIn*)obj);
            break;
        case T_LetRec:
            letRec_markChildren((struct E_LetRec*)obj);
            break;
        case T_List:
            list_markChildren((struct D_List*)obj);
            break;
        case T_Nil:
            _mark(obj);
            break;
        case T_Primitive:
            _mark(obj);
            break;
        case T_Protocol:
            protocol_markChildren((struct E_Protocol*)obj);
            break;
        case T_Queue:
            queue_markChildren((struct D_Queue*)obj);
            break;
        case T_Real:
            _mark(obj);
            break;
        case T_Record:
            record_markChildren((struct D_Record*)obj);
            break;
        case T_RecordDefinition:
            recordDefinition_markChildren((struct E_RecordDefinition*)obj);
            break;
        case T_RecordSpec:
            recordSpec_markChildren((struct E_RecordSpec*)obj);
            break;
        case T_REPL:
            repl_markChildren((struct REPL*)obj);
            break;
        case T_SavedEnv:
            savedEnv_markChildren((struct E_SavedEnv*)obj);
            break;
        case T_Sequence:
            _mark(obj);
            break;
        case T_Set:
            set_markChildren((struct D_Set*)obj);
            break;
        case T_String:
            _mark(obj);
            break;
        case T_StringBuffer:
            _mark(obj);
            break;
        case T_StringStream:
            _mark(obj);
            break;
        case T_Symbol:
            _mark(obj);
            break;
        case T_Term:
            term_markChildren((struct D_Term*)obj);
            break;
        case T_Triple:
            triple_markChildren((struct D_Triple*)obj);
            break;
        case T_TryCatch:
            tryCatch_markChildren((struct E_TryCatch*)obj);
            break;
        case T_Tuple:
            tuple_markChildren((struct D_Tuple*)obj);
            break;
        case T_Unsigned:
            _mark(obj);
            break;
        case T_FINAL:
            _error(obj);
            break;
        default:
            break;
    }
}
