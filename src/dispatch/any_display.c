#include <stdlib.h>

#include "data/any.h"
#include "data/string.h"
#include "data/unsigned.h"
#include "dispatch/methodtable.h"
        
static void _error(struct Any* obj, FILE* fp) {
    (void)fp;
    fprintf(stderr, "ERROR: call to 'any_display' is not valid for type ID %d (%0x)\n", obj->typeId, obj->typeId);
    exit(1);
}

void any_display(struct Any* obj, FILE* fp) {
#if 1
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    if (methods != NULL) {
        void (*method)(struct Any*, FILE* fp) = methods->m_display;
        if (method != NULL) {
            method(obj, fp);
            return;
        }
    }
    printf("%s no method to handle typeId %d %s\n", __func__, obj->typeId, TYPE_NAMES[obj->typeId]);
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
        case T_Address:
        case T_Binding:
        case T_BinOp:
        case T_Apply:
        case T_Array:
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
        case T_Iterator:
        case T_Let:
        case T_LetIn:
        case T_LetRec:
        case T_List:
        case T_Nil:
        case T_Primitive:
        case T_Protocol:
        case T_Queue:
        case T_Real:
        case T_Record:
        case T_RecordDefinition:
        case T_RecordSpec:
        case T_REPL:
        case T_SavedEnv:
        case T_Sequence:
        case T_Set:
        case T_StringBuffer:
        case T_StringStream:
        case T_Symbol:
        case T_Term:
        case T_Triple:
        case T_TryCatch:
        case T_Tuple:
            any_show(obj, fp);
            break;
        case T_String:
            string_display((struct D_String*)obj, fp);
            break;
        case T_Unsigned:
            unsigned_display((struct D_Unsigned*)obj, fp);
            break;
        case T_FINAL:
            _error(obj, fp);
            break;
        default:
            break;
    }
}
