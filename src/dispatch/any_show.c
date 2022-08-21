#include <stdlib.h>

#include "data/address.h"
#include "data/any.h"
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
        
static void _error(struct Any* obj, FILE* fp) {
    (void)fp;
    fprintf(stderr, "ERROR: call to 'any_show' is not valid for type ID %d (%0x)\n", obj->typeId, obj->typeId);
    exit(1);
}

void any_show(struct Any* obj, FILE* fp) {
#if 1
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    if (methods != NULL) {
        void (*method)(struct Any*, FILE* fp) = methods->m_show;
        if (method != NULL) {
            method(obj, fp);
            return;
        }
    }
    printf("%s no method to handle typeId %d\n", __func__, obj->typeId);
#endif
    if (obj == NULL) {
        fputs("NULL", fp);
        return;
    }
    switch (obj->typeId) {
        case T_NULL:
            _error(obj, fp);
            break;
        case T_Abstraction:
            abstraction_show((struct E_Abstraction*)obj, fp);
            break;
        case T_Address:
            address_show((struct D_Address*)obj, fp);
            break;
        case T_Binding:
            binding_show((struct D_Binding*)obj, fp);
            break;
        case T_BinOp:
            binOp_show((struct E_BinOp*)obj, fp);
            break;
        case T_Apply:
            apply_show((struct E_Apply*)obj, fp);
            break;
        case T_Array:
            array_show((struct D_Array*)obj, fp);
            break;
        case T_Boolean:
            boolean_show((struct D_Boolean*)obj, fp);
            break;
        case T_BracketExpr:
            bracketExpr_show((struct E_BracketExpr*)obj, fp);
            break;
        case T_Closure:
            closure_show((struct D_Closure*)obj, fp);
            break;
        case T_Cobegin:
            cobegin_show((struct E_Cobegin*)obj, fp);
            break;
        case T_Continuation:
            continuation_show((struct E_Continuation*)obj, fp);
            break;
        case T_Do:
            do_show((struct E_Do*)obj, fp);
            break;
        case T_Evaluator:
            evaluator_show((struct Evaluator*)obj, fp);
            break;
        case T_ExceptionHandler:
            exceptionHandler_show((struct E_ExceptionHandler*)obj, fp);
            break;
        case T_File:
            file_show((struct D_File*)obj, fp);
            break;
        case T_HashTable:
            hashTable_show((struct D_HashTable*)obj, fp);
            break;
        case T_Identifier:
            identifier_show((struct E_Identifier*)obj, fp);
            break;
        case T_If:
            if_show((struct E_If*)obj, fp);
            break;
        case T_Integer:
            integer_show((struct D_Integer*)obj, fp);
            break;
        case T_Iterator:
            iterator_show((struct D_Iterator*)obj, fp);
            break;
        case T_Let:
            let_show((struct E_Let*)obj, fp);
            break;
        case T_LetIn:
            letIn_show((struct E_LetIn*)obj, fp);
            break;
        case T_LetRec:
            letRec_show((struct E_LetRec*)obj, fp);
            break;
        case T_List:
            list_show((struct D_List*)obj, fp);
            break;
        case T_Nil:
            nil_show((struct D_Nil*)obj, fp);
            break;
        case T_Primitive:
            primitive_show((struct D_Primitive*)obj, fp);
            break;
        case T_Protocol:
            protocol_show((struct E_Protocol*)obj, fp);
            break;
        case T_Queue:
            queue_show((struct D_Queue*)obj, fp);
            break;
        case T_Real:
            real_show((struct D_Real*)obj, fp);
            break;
        case T_Record:
            record_show((struct D_Record*)obj, fp);
            break;
        case T_RecordDefinition:
            recordDefinition_show((struct E_RecordDefinition*)obj, fp);
            break;
        case T_RecordSpec:
            recordSpec_show((struct E_RecordSpec*)obj, fp);
            break;
        case T_REPL:
            repl_show((struct REPL*)obj, fp);
            break;
        case T_SavedEnv:
            savedEnv_show((struct E_SavedEnv*)obj, fp);
            break;
        case T_Sequence:
            sequence_show((struct D_Sequence*)obj, fp);
            break;
        case T_Set:
            set_show((struct D_Set*)obj, fp);
            break;
        case T_String:
            string_show((struct D_String*)obj, fp);
            break;
        case T_StringBuffer:
            stringBuffer_show((struct D_StringBuffer*)obj, fp);
            break;
        case T_StringStream:
            stringStream_show((struct D_StringStream*)obj, fp);
            break;
        case T_Symbol:
            symbol_show((struct D_Symbol*)obj, fp);
            break;
        case T_Term:
            term_show((struct D_Term*)obj, fp);
            break;
        case T_Triple:
            triple_show((struct D_Triple*)obj, fp);
            break;
        case T_TryCatch:
            tryCatch_show((struct E_TryCatch*)obj, fp);
            break;
        case T_Tuple:
            tuple_show((struct D_Tuple*)obj, fp);
            break;
        case T_Unsigned:
            unsigned_show((struct D_Unsigned*)obj, fp);
            break;
        case T_FINAL:
            _error(obj, fp);
            break;
        default:
            break;
    }
}
