#include <stdlib.h>

#include "data/any.h"
#include "methods/methods.h"

struct Methods* abstraction_methodSetup();
struct Methods* address_methodSetup();
struct Methods* apply_methodSetup();
struct Methods* array_methodSetup();
struct Methods* binOp_methodSetup();
struct Methods* binding_methodSetup();
struct Methods* boolean_methodSetup();
struct Methods* bracketExpr_methodSetup();
struct Methods* closure_methodSetup();
struct Methods* cobegin_methodSetup();
struct Methods* continuation_methodSetup();
struct Methods* do_methodSetup();
struct Methods* evaluator_methodSetup();
struct Methods* exceptionHandler_methodSetup();
struct Methods* file_methodSetup();
struct Methods* hashTable_methodSetup();
struct Methods* identifier_methodSetup();
struct Methods* if_methodSetup();
struct Methods* integer_methodSetup();
struct Methods* iterator_methodSetup();
struct Methods* let_methodSetup();
struct Methods* letIn_methodSetup();
struct Methods* letRec_methodSetup();
struct Methods* list_methodSetup();
struct Methods* loop_methodSetup();
struct Methods* nil_methodSetup();
struct Methods* primitive_methodSetup();
struct Methods* protocol_methodSetup();
struct Methods* queue_methodSetup();
struct Methods* repl_methodSetup();
struct Methods* real_methodSetup();
struct Methods* record_methodSetup();
struct Methods* recordDefinition_methodSetup();
struct Methods* recordSpec_methodSetup();
struct Methods* savedEnv_methodSetup();
struct Methods* sequence_methodSetup();
struct Methods* set_methodSetup();
struct Methods* stream_methodSetup();
struct Methods* string_methodSetup();
struct Methods* stringBuffer_methodSetup();
struct Methods* stringStream_methodSetup();
struct Methods* symbol_methodSetup();
struct Methods* term_methodSetup();
struct Methods* triple_methodSetup();
struct Methods* tryCatch_methodSetup();
struct Methods* tuple_methodSetup();
struct Methods* unsigned_methodSetup();

// called from main.c
void methodTable_setupMethods(void) {
    METHOD_TABLE[T_Abstraction] = abstraction_methodSetup();
    METHOD_TABLE[T_Address] = address_methodSetup();
    METHOD_TABLE[T_Apply] = apply_methodSetup();
    METHOD_TABLE[T_Array] = array_methodSetup();
    METHOD_TABLE[T_BinOp] = binOp_methodSetup();
    METHOD_TABLE[T_Binding] = binding_methodSetup();
    METHOD_TABLE[T_Boolean] = boolean_methodSetup();
    METHOD_TABLE[T_BracketExpr] = bracketExpr_methodSetup();
    METHOD_TABLE[T_Closure] = closure_methodSetup();
    METHOD_TABLE[T_Cobegin] = cobegin_methodSetup();
    METHOD_TABLE[T_Continuation] = continuation_methodSetup();
    METHOD_TABLE[T_Do] = do_methodSetup();
    METHOD_TABLE[T_Evaluator] = evaluator_methodSetup();
    METHOD_TABLE[T_ExceptionHandler] = exceptionHandler_methodSetup();
    METHOD_TABLE[T_File] = file_methodSetup();
    METHOD_TABLE[T_HashTable] = hashTable_methodSetup();
    METHOD_TABLE[T_Identifier] = identifier_methodSetup();
    METHOD_TABLE[T_If] = if_methodSetup();
    METHOD_TABLE[T_Integer] = integer_methodSetup();
    METHOD_TABLE[T_Iterator] = iterator_methodSetup();
    METHOD_TABLE[T_Let] = let_methodSetup();
    METHOD_TABLE[T_LetIn] = letIn_methodSetup();
    METHOD_TABLE[T_LetRec] = letRec_methodSetup();
    METHOD_TABLE[T_Loop] = loop_methodSetup();
    METHOD_TABLE[T_List] = list_methodSetup();
    METHOD_TABLE[T_Nil] = nil_methodSetup();
    METHOD_TABLE[T_Primitive] = primitive_methodSetup();
    METHOD_TABLE[T_Protocol] = protocol_methodSetup();
    METHOD_TABLE[T_Queue] = queue_methodSetup();
    METHOD_TABLE[T_REPL] = repl_methodSetup();
    METHOD_TABLE[T_Real] = real_methodSetup();
    METHOD_TABLE[T_Record] = record_methodSetup();
    METHOD_TABLE[T_RecordDefinition] = recordDefinition_methodSetup();
    METHOD_TABLE[T_RecordSpec] = recordSpec_methodSetup();
    METHOD_TABLE[T_SavedEnv] = savedEnv_methodSetup();
    METHOD_TABLE[T_Sequence] = sequence_methodSetup();
    METHOD_TABLE[T_Set] = set_methodSetup();
    METHOD_TABLE[T_Stream] = stream_methodSetup();
    METHOD_TABLE[T_String] = string_methodSetup();
    METHOD_TABLE[T_StringBuffer] = stringBuffer_methodSetup();
    METHOD_TABLE[T_StringStream] = stringStream_methodSetup();
    METHOD_TABLE[T_Symbol] = symbol_methodSetup();
    METHOD_TABLE[T_Term] = term_methodSetup();
    METHOD_TABLE[T_Triple] = triple_methodSetup();
    METHOD_TABLE[T_TryCatch] = tryCatch_methodSetup();
    METHOD_TABLE[T_Tuple] = tuple_methodSetup();
    METHOD_TABLE[T_Unsigned] = unsigned_methodSetup();
}

void methodTable_deallocateAll(void) {
    for (int n=0; n<T_FINAL; n++) {
        free(METHOD_TABLE[n]);
    }
}
