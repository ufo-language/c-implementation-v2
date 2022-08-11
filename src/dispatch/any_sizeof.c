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

size_t sequence_sizeOf(struct D_Sequence*);

static size_t _error(struct Any* obj) {
    fprintf(stderr, "ERROR: call to 'any_sizeof' is not valid for type ID %d (%0x)\n", obj->typeId, obj->typeId);
    exit(1);
    return 0;
}

size_t any_sizeOf(struct Any* obj) {
    switch (obj->typeId) {
        case T_NULL:
            return _error(obj);
        case T_Abstraction:
            return abstraction_sizeOf((struct E_Abstraction*)obj);
        case T_Address:
            return address_sizeOf((struct D_Address*)obj);
        case T_Apply:
            return apply_sizeOf((struct E_Apply*)obj);
        case T_Array:
            return array_sizeOf((struct D_Array*)obj);
        case T_Binding:
            return binding_sizeOf((struct D_Binding*)obj);
        case T_BinOp:
            return binOp_sizeOf((struct E_BinOp*)obj);
        case T_Boolean:
            return boolean_sizeOf((struct D_Boolean*)obj);
        case T_BracketExpr:
            return bracketExpr_sizeOf((struct E_BracketExpr*)obj);
        case T_Closure:
            return closure_sizeOf((struct D_Closure*)obj);
        case T_Continuation:
            return continuation_sizeOf((struct E_Continuation*)obj);
        case T_Cobegin:
            return cobegin_sizeOf((struct E_Cobegin*)obj);
        case T_Do:
            return do_sizeOf((struct E_Do*)obj);
        case T_Evaluator:
            return evaluator_sizeOf((struct Evaluator*)obj);
        case T_ExceptionHandler:
            return exceptionHandler_sizeOf((struct E_ExceptionHandler*)obj);
        case T_File:
            return file_sizeOf((struct D_File*)obj);
        case T_HashTable:
            return hashTable_sizeOf((struct D_HashTable*)obj);
        case T_Identifier:
            return identifier_sizeOf((struct E_Identifier*)obj);
        case T_If:
            return if_sizeOf((struct E_If*)obj);
        case T_Integer:
            return integer_sizeOf((struct D_Integer*)obj);
        case T_LetIn:
            return letIn_sizeOf((struct E_LetIn*)obj);
        case T_Let:
            return let_sizeOf((struct E_Let*)obj);
        case T_LetRec:
            return letRec_sizeOf((struct E_LetRec*)obj);
        case T_List:
            return list_sizeOf((struct D_List*)obj);
        case T_Nil:
            return nil_sizeOf((struct D_Nil*)obj);
        case T_Primitive:
            return primitive_sizeOf((struct D_Primitive*)obj);
        case T_Protocol:
            return protocol_sizeOf((struct E_Protocol*)obj);
        case T_Queue:
            return queue_sizeOf((struct D_Queue*)obj);
        case T_Real:
            return real_sizeOf((struct D_Real*)obj);
        case T_Record:
            return record_sizeOf((struct D_Record*)obj);
        case T_RecordDefinition:
            return recordDefinition_sizeOf((struct E_RecordDefinition*)obj);
        case T_RecordSpec:
            return recordSpec_sizeOf((struct E_RecordSpec*)obj);
        case T_REPL:
            return repl_sizeOf((struct REPL*)obj);
        case T_SavedEnv:
            return savedEnv_sizeOf((struct E_SavedEnv*)obj);
        case T_Sequence:
            return sequence_sizeOf((struct D_Sequence*)obj);
        case T_Set:
            return set_sizeOf((struct D_Set*)obj);
        case T_String:
            return string_sizeOf((struct D_String*)obj);
        case T_StringBuffer:
            return stringBuffer_sizeOf((struct D_StringBuffer*)obj);
        case T_StringStream:
            return stringStream_sizeOf((struct D_StringStream*)obj);
        case T_Symbol:
            return symbol_sizeOf((struct D_Symbol*)obj);
        case T_Term:
            return term_sizeOf((struct D_Term*)obj);
        case T_Triple:
            return triple_sizeOf((struct D_Triple*)obj);
        case T_TryCatch:
            return tryCatch_sizeOf((struct E_TryCatch*)obj);
        case T_Tuple:
            return tuple_sizeOf((struct D_Tuple*)obj);
        case T_Unsigned:
            return unsigned_sizeOf((struct D_Unsigned*)obj);
        case T_FINAL:
            return _error(obj);
        default:
            return 0;
    }
}
