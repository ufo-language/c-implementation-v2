#if !defined(ANY_H)
#define ANY_H

#include <stdbool.h>
#include <stdio.h>

#include "main/typedefs.h"  // for HashCode

enum TypeId {
    // T_NULL is not a type in UFO, but this entry is used for a few things.
    T_NULL = 0,
    T_Abstraction,
    T_Address,
    T_Apply,
    T_Array,
    T_Async,
    T_BinOp,
    T_Binding,
    T_Boolean,
    T_BracketExpr,
    T_Closure,
    T_Cobegin,
    T_Continuation,
    T_Do,
    T_Evaluator,
    T_ExceptionHandler,
    T_File,
    T_HashTable,
    T_Identifier,
    T_If,
    T_Integer,
    T_Iterator,
    T_Let,
    T_LetIn,
    T_LetRec,
    T_Lock,
    T_Loop,
    T_List,
    T_MsgQ,
    T_Nil,
    T_Nondet,
    T_Primitive,
    T_Protocol,
    T_Queue,
    T_Quote,
    T_REPL,
    T_Real,
    T_Record,
    T_RecordDefinition,
    T_RecordSpec,
    T_SavedEnv,
    T_Sequence,
    T_Set,
    T_Stream,
    T_String,
    T_StringBuffer,
    T_StringStream,
    T_Symbol,
    T_Term,
    T_Triple,
    T_TryCatch,
    T_Tuple,
    T_Unsigned,
    // T_FINAL is not a type in UFO, but this entry is used to do some
    // sanity checking on the whole table of types.
    T_FINAL
};

extern char* TYPE_NAMES[T_FINAL + 1];

struct Evaluator;
struct D_Iterator;
struct D_List;
struct D_Set;
struct D_Symbol;
struct MethodTable;

struct Any {
    enum TypeId typeId;
    struct Any* next;
    bool isMarked;
};

void any_free(struct Any* obj);

// these functions are defined in any.c
bool any_hasType(struct Any* obj, struct Any* typeObj);
bool any_isA(struct Any* obj, enum TypeId typeId);
bool any_isMarked(struct Any* obj);
enum TypeId any_typeId(struct Any* obj);
char* any_typeName(struct Any* obj);
struct D_Symbol* any_typeSymbol(struct Any* obj);

// these functions are defined in the src/dispatch directory
bool any_boolValue(struct Any* obj);
int any_compare(struct Any* obj, struct Any* other, struct Evaluator* etor);
struct Any* any_deepCopy(struct Any* obj);
void any_display(struct Any* obj, FILE* fp);
void any_eval(struct Any* obj, struct Evaluator* etor);
void any_freeVars(struct Any* obj, struct D_Set* freeVars, struct Evaluator* etor);
struct Any* any_getPairValue(struct Any* obj, struct Any* key);
bool any_hashCode(struct Any* obj, HashCode* hashCode);
bool any_isEqual(struct Any* obj, struct Any* other);
struct D_Iterator* any_iterator(struct Any* obj);
void any_mark(struct Any* obj);
struct D_Triple* any_match(struct Any* obj, struct Any* other, struct D_Triple* bindings);
void any_show(struct Any* obj, FILE* fp);
size_t any_sizeOf(struct Any* obj);
size_t any_structSize(enum TypeId typeId);
struct Any* any_typeOf(struct Any* obj);

#endif
