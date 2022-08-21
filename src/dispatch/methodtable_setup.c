#include "data/any.h"
#include "dispatch/methodtable.h"

struct Methods* identifier_methodSetup();
struct Methods* iterator_methodSetup();
struct Methods* string_methodSetup();

// called from main.c
void methodTable_setupMethods(void) {
    //METHOD_TABLE[T_Abstraction] = _methodSetup();
    //METHOD_TABLE[T_Address] = _methodSetup();
    //METHOD_TABLE[T_Apply] = _methodSetup();
    //METHOD_TABLE[T_Array] = _methodSetup();
    //METHOD_TABLE[T_BinOp] = _methodSetup();
    //METHOD_TABLE[T_Binding] = _methodSetup();
    //METHOD_TABLE[T_Boolean] = _methodSetup();
    //METHOD_TABLE[T_BracketExpr] = _methodSetup();
    //METHOD_TABLE[T_Closure] = _methodSetup();
    //METHOD_TABLE[T_Cobegin] = _methodSetup();
    //METHOD_TABLE[T_Continuation] = _methodSetup();
    //METHOD_TABLE[T_Do] = _methodSetup();
    //METHOD_TABLE[T_Evaluator] = _methodSetup();
    //METHOD_TABLE[T_ExceptionHandler] = _methodSetup();
    //METHOD_TABLE[T_File] = _methodSetup();
    //METHOD_TABLE[T_HashTable] = _methodSetup();
    METHOD_TABLE[T_Identifier] = identifier_methodSetup();
    //METHOD_TABLE[T_If] = _methodSetup();
    //METHOD_TABLE[T_Integer] = _methodSetup();
    METHOD_TABLE[T_Iterator] = iterator_methodSetup();
    //METHOD_TABLE[T_Let] = _methodSetup();
    //METHOD_TABLE[T_LetIn] = _methodSetup();
    //METHOD_TABLE[T_LetRec] = _methodSetup();
    //METHOD_TABLE[T_List] = _methodSetup();
    //METHOD_TABLE[T_Nil] = _methodSetup();
    //METHOD_TABLE[T_Primitive] = _methodSetup();
    //METHOD_TABLE[T_Protocol] = _methodSetup();
    //METHOD_TABLE[T_Queue] = _methodSetup();
    //METHOD_TABLE[T_REPL] = _methodSetup();
    //METHOD_TABLE[T_Real] = _methodSetup();
    //METHOD_TABLE[T_Record] = _methodSetup();
    //METHOD_TABLE[T_RecordDefinition] = _methodSetup();
    //METHOD_TABLE[T_RecordSpec] = _methodSetup();
    //METHOD_TABLE[T_SavedEnv] = _methodSetup();
    //METHOD_TABLE[T_Sequence] = _methodSetup();
    //METHOD_TABLE[T_Set] = _methodSetup();
    METHOD_TABLE[T_String] = string_methodSetup();
    //METHOD_TABLE[T_StringBuffer] = _methodSetup();
    //METHOD_TABLE[T_StringStream] = _methodSetup();
    METHOD_TABLE[T_Symbol] = symbol_methodSetup();
    //METHOD_TABLE[T_Term] = _methodSetup();
    //METHOD_TABLE[T_Triple] = _methodSetup();
    //METHOD_TABLE[T_TryCatch] = _methodSetup();
    //METHOD_TABLE[T_Tuple] = _methodSetup();
    //METHOD_TABLE[T_Unsigned] = _methodSetup();
}
