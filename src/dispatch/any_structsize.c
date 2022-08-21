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

static size_t _error(enum TypeId typeId) {
    fprintf(stderr, "ERROR: call to 'any_structSize' called with illegal typeId %d\n", typeId);
    exit(1);
    return 0;
}

size_t any_structSize(enum TypeId typeId) {
#if 0
    struct Methods* methods = METHOD_TABLE[typeId];
    if (methods != NULL) {
        size_t (*method)(enum TypeId) = methods->m_structSize;
        printf("%s %s method = %p\n", __func__, TYPE_NAMES[typeId], (void*)method);
        if (method != NULL) {
            size_t size = method(typeId);
            printf("  got size %lu\n", size);
            return size;
        }
    }
    printf("%s no method to handle typeId %d %s\n", __func__, typeId, TYPE_NAMES[typeId]);
#endif
    switch (typeId) {
        case T_Abstraction:
            return abstraction_structSize();
        case T_Address:
            return address_structSize();
        case T_Apply:
            return apply_structSize();
        case T_Array:
            return array_structSize();
        case T_Binding:
            return binding_structSize();
        case T_BinOp:
            return binOp_structSize();
        case T_Boolean:
            return boolean_structSize();
        case T_BracketExpr:
            return bracketExpr_structSize();
        case T_Closure:
            return closure_structSize();
        case T_Cobegin:
            return cobegin_structSize();
        case T_Continuation:
            return continuation_structSize();
        case T_Do:
            return do_structSize();
        case T_Evaluator:
            return evaluator_structSize();
        case T_ExceptionHandler:
            return exceptionHandler_structSize();
        case T_File:
            return file_structSize();
        case T_HashTable:
            return hashTable_structSize();
        case T_Identifier:
            return identifier_structSize(typeId);
        case T_If:
            return if_structSize();
        case T_Integer:
            return integer_structSize();
        case T_Let:
            return let_structSize();
        case T_LetIn:
            return letIn_structSize();
        case T_LetRec:
            return letRec_structSize();
        case T_List:
            return list_structSize();
        case T_Nil:
            return nil_structSize();
        case T_Primitive:
            return primitive_structSize();
        case T_Protocol:
            return protocol_structSize();
        case T_Queue:
            return queue_structSize();
        case T_Real:
            return real_structSize();
        case T_Record:
            return record_structSize();
        case T_RecordDefinition:
            return recordDefinition_structSize();
        case T_RecordSpec:
            return recordSpec_structSize();
        case T_REPL:
            return repl_structSize();
        case T_SavedEnv:
            return savedEnv_structSize();
        case T_Sequence:
            return sequence_structSize();
        case T_Set:
            return set_structSize();
        case T_String:
            return string_structSize(typeId);
        case T_StringBuffer:
            return stringBuffer_structSize();
        case T_StringStream:
            return stringStream_structSize();
        case T_Symbol:
            return symbol_structSize(typeId);
        case T_Term:
            return term_structSize();
        case T_Triple:
            return triple_structSize();
        case T_TryCatch:
            return tryCatch_structSize();
        case T_Tuple:
            return tuple_structSize();
        case T_Unsigned:
            return unsigned_structSize();
        case T_NULL:
        case T_Iterator:
        case T_FINAL:
        default:
            return _error(typeId);
    }
}
