#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/primitive.h"
#include "data/symbol.h"
#include "data/unsigned.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "array"

static void _count(struct Evaluator* etor, struct D_List* args);
static void _new(struct Evaluator* etor, struct D_List* args);
void ns_array_get(struct Evaluator* etor, struct D_List* args);
static void _set(struct Evaluator* etor, struct D_List* args);

void ns_array_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "count", _count);
    primitive_define(nsHash, "get", ns_array_get);
    primitive_define(nsHash, "new", _new);
    primitive_define(nsHash, "set", _set);
}

static void _count(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Array};
    struct Any* arrayObj;
    struct Any** paramVars[] = {&arrayObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Array* array = (struct D_Array*)arrayObj;
    struct D_Unsigned* countNum = unsigned_new(array_count(array));
    evaluator_pushObj(etor, (struct Any*)countNum);
}

static void _new(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Integer, T_NULL};
    struct Any* countObj;
    struct Any* initialValue = (struct Any*)NIL;
    struct Any** paramVars[] = {&countObj, &initialValue};
    primitive_checkArgs2(1, 2, paramTypes, args, paramVars, etor);
    int count = integer_getValue((struct D_Integer*)countObj);
    struct D_Array* ary = (struct D_Array*)array_new(count);
    for (int n=0; n<count; n++) {
        array_set_unsafe(ary, n, initialValue);
    }
    evaluator_pushObj(etor, (struct Any*)ary);
}

void ns_array_get(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Array, T_Integer};
    struct Any* arrayObj;
    struct Any* countObj;
    struct Any** paramVars[] = {&arrayObj, &countObj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_Array* array = (struct D_Array*)arrayObj;
    struct D_Unsigned* count = (struct D_Unsigned*)countObj;
    int i = unsigned_getValue(count);
    // TODO replace with call to array_get (the non-unsafe function)
    if (i >= array_count(array)) {
        struct D_Symbol* sym = symbol_new("Array");
        struct D_Array* exn = array_newN(2, count, array);
        evaluator_throwException(etor, sym, "index out of bounds", (struct Any*)exn);
    }
    evaluator_pushObj(etor, (struct Any*)array_get_unsafe(array, i));
}

static void _set(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Array, T_Unsigned, T_NULL};
    struct Any* arrayObj;
    struct Any* countObj;
    struct Any* elem;
    struct Any** paramVars[] = {&arrayObj, &countObj, &elem};
    primitive_checkArgs(3, paramTypes, args, paramVars, etor);
    struct D_Array* array = (struct D_Array*)arrayObj;
    struct D_Unsigned* count = (struct D_Unsigned*)countObj;
    int i = unsigned_getValue(count);
    // TODO replace with call to array_get (the non-unsafe function)
    if (i >= array_count(array)) {
        struct D_Symbol* sym = symbol_new("Array");
        struct D_Array* exn = array_newN(2, count, array);
        evaluator_throwException(etor, sym, "index out of bounds", (struct Any*)exn);
    }
    array_set_unsafe(array, i, elem);
    evaluator_pushObj(etor, (struct Any*)array);
}
