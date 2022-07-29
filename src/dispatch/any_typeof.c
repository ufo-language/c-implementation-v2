#include <stdlib.h>

// TODO most of these includes are no longer needed
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
#include "data/nothing.h"
#include "data/primitive.h"
#include "data/queue.h"
#include "data/real.h"
#include "data/record.h"
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

static struct Any* _error(struct Any* obj) {
    fprintf(stderr, "ERROR: call to 'any_typeOf' is not valid for type ID %d (%0x)\n", obj->typeId, obj->typeId);
    exit(1);
    return NULL;
}

struct Any* any_typeOf(struct Any* obj) {
    switch (obj->typeId) {
    case T_NULL:
        return _error(obj);
    case T_Abstraction:
    case T_Address:
    case T_Apply:
    case T_Array:
    case T_Binding:
    case T_BinOp:
    case T_Boolean:
    case T_BracketExpr:
    case T_Closure:
    case T_Cobegin:
    case T_Continuation:
    case T_Do:
    case T_Evaluator:
    case T_ExceptionHandler:
    case T_File:
    case T_HashTable:
    case T_Identifier:
    case T_If:
    case T_Integer:
    case T_Let:
    case T_LetIn:
    case T_LetRec:
    case T_List:
    case T_Nothing:
    case T_Primitive:
    case T_Protocol:
    case T_Queue:
    case T_Real:
    case T_RecordDefinition:
    case T_RecordSpec:
    case T_REPL:
    case T_SavedEnv:
    case T_Set:
    case T_String:
    case T_StringBuffer:
    case T_StringStream:
    case T_Symbol:
    case T_Term:
    case T_Triple:
    case T_TryCatch:
    case T_Tuple:
    case T_Unsigned:
        return (struct Any*)any_typeSymbol(obj);
    case T_Record:
        return record_typeOf((struct D_Record*)obj);
    case T_FINAL:
        return _error(obj);
    default:
        return 0;
    }
}

