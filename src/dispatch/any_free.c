#include <stdio.h>
#include <stdlib.h>

#include "data/address.h"
#include "data/array.h"
#include "data/binding.h"
#include "data/boolean.h"
#include "data/closure.h"
#include "data/file.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/iterator.h"
#include "data/list.h"
#include "data/nil.h"
#include "data/primitive.h"
#include "data/queue.h"
#include "data/real.h"
#include "data/record.h"
#include "data/sequence.h"
#include "data/set.h"
#include "data/string.h"
#include "data/stringbuffer.h"
#include "data/stringstream.h"
#include "data/symbol.h"
#include "data/term.h"
#include "data/triple.h"
#include "data/tuple.h"
#include "data/unsigned.h"
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
#include "repl/repl.h"

static bool _error(struct Any* obj) {
    fprintf(stderr, "ERROR: call to 'any_free' is not valid for type ID %d (%0x)\n", obj->typeId, obj->typeId);
    exit(1);
    return false;
}

void any_free(struct Any* obj) {
#if 1
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    if (methods != NULL) {
        void (*method)(struct Any*) = methods->m_free;
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
            abstraction_free((struct E_Abstraction*)obj);
            break;
        case T_Address:
            address_free((struct D_Address*)obj);
            break;
        case T_Apply:
            apply_free((struct E_Apply*)obj);
            break;
        case T_Array:
            array_free((struct D_Array*)obj);
            break;
        case T_Binding:
            binding_free((struct D_Binding*)obj);
            break;
        case T_BinOp:
            binOp_free((struct E_BinOp*)obj);
            break;
        case T_Boolean:
            boolean_free((struct D_Boolean*)obj);
            break;
        case T_BracketExpr:
            bracketExpr_free((struct E_BracketExpr*)obj);
            break;
        case T_Closure:
            closure_free((struct D_Closure*)obj);
            break;
        case T_Cobegin:
            cobegin_free((struct E_Cobegin*)obj);
            break;
        case T_Continuation:
            continuation_free((struct E_Continuation*)obj);
            break;
        case T_Do:
            do_free((struct E_Do*)obj);
            break;
        case T_Evaluator:
            evaluator_free((struct Evaluator*)obj);
            break;
        case T_ExceptionHandler:
            exceptionHandler_free((struct E_ExceptionHandler*)obj);
            break;
        case T_File:
            file_free((struct D_File*)obj);
            break;
        case T_HashTable:
            hashTable_free((struct D_HashTable*)obj);
            break;
        case T_Identifier:
            identifier_free((struct E_Identifier*)obj);
            break;
        case T_Integer:
            integer_free((struct D_Integer*)obj);
            break;
        case T_Iterator:
            iterator_free((struct D_Iterator*)obj);
            break;
        case T_If:
            if_free((struct E_If*)obj);
            break;
        case T_Let:
            let_free((struct E_Let*)obj);
            break;
        case T_LetIn:
            letIn_free((struct E_LetIn*)obj);
            break;
        case T_LetRec:
            letRec_free((struct E_LetRec*)obj);
            break;
        case T_List:
            list_free((struct D_List*)obj);
            break;
        case T_Nil:
            nil_free((struct D_Nil*)obj);
            break;
        case T_Primitive:
            primitive_free((struct D_Primitive*)obj);
            break;
        case T_Protocol:
            protocol_free((struct E_Protocol*)obj);
            break;
        case T_Queue:
            queue_free((struct D_Queue*)obj);
            break;
        case T_Real:
            real_free((struct D_Real*)obj);
            break;
        case T_Record:
            record_free((struct D_Record*)obj);
            break;
        case T_RecordDefinition:
            recordDefinition_free((struct E_RecordDefinition*)obj);
            break;
        case T_RecordSpec:
            recordSpec_free((struct E_RecordSpec*)obj);
            break;
        case T_REPL:
            repl_free((struct REPL*)obj);
            break;
        case T_SavedEnv:
            savedEnv_free((struct E_SavedEnv*)obj);
            break;
        case T_Sequence:
            sequence_free((struct D_Sequence*)obj);
            break;
        case T_Set:
            set_free((struct D_Set*)obj);
            break;
        case T_Symbol:
            symbol_free((struct D_Symbol*)obj);
            break;
        case T_String:
            string_free((struct D_String*)obj);
            break;
        case T_StringBuffer:
            stringBuffer_free((struct D_StringBuffer*)obj);
            break;
        case T_StringStream:
            stringStream_free((struct D_StringStream*)obj);
            break;
        case T_Term:
            term_free((struct D_Term*)obj);
            break;
        case T_Triple:
            triple_free((struct D_Triple*)obj);
            break;
        case T_Tuple:
            tuple_free((struct D_Tuple*)obj);
            break;
        case T_TryCatch:
            tryCatch_free((struct E_TryCatch*)obj);
            break;
        case T_Unsigned:
            unsigned_free((struct D_Unsigned*)obj);
            break;
        case T_FINAL:
            _error(obj);
            break;
        default:
            return;
    }
}
