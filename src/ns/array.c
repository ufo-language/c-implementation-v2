#include "data/any.h"
#include "data/array.h"
#include "data/boolean.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/primitive.h"
#include "data/sequence.h"
#include "data/symbol.h"
#include "data/unsigned.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "array"

static void _asQueue(struct Evaluator* etor, struct D_List* args);
static void _contains(struct Evaluator* etor, struct D_List* args);
static void _count(struct Evaluator* etor, struct D_List* args);
static void _delete(struct Evaluator* etor, struct D_List* args);
static void _domain(struct Evaluator* etor, struct D_List* args);
static void _insert(struct Evaluator* etor, struct D_List* args);
static void _insertionSort(struct Evaluator* etor, struct D_List* args);
static void _map(struct Evaluator* etor, struct D_List* args);
static void _new(struct Evaluator* etor, struct D_List* args);
void ns_array_get(struct Evaluator* etor, struct D_List* args);
static void _reverse(struct Evaluator* etor, struct D_List* args);
static void _set(struct Evaluator* etor, struct D_List* args);
static void _selectionSort(struct Evaluator* etor, struct D_List* args);
static void _shuffle(struct Evaluator* etor, struct D_List* args);

void ns_array_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "asQueue", _asQueue);
    primitive_define(nsHash, "contains", _contains);
    primitive_define(nsHash, "count", _count);
    primitive_define(nsHash, "delete", _delete);
    primitive_define(nsHash, "domain", _domain);
    primitive_define(nsHash, "get", ns_array_get);  // used by src/expr/bracketexpr.c
    primitive_define(nsHash, "insert", _insert);
    primitive_define(nsHash, "insertionSort", _insertionSort);
    primitive_define(nsHash, "map", _map);
    primitive_define(nsHash, "new", _new);
    primitive_define(nsHash, "reverse", _reverse);
    primitive_define(nsHash, "set", _set);
    primitive_define(nsHash, "selectionSort", _selectionSort);
    primitive_define(nsHash, "shuffle", _shuffle);
}

static void _asQueue(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Array};
    struct Any* arrayObj;
    struct Any** paramVars[] = {&arrayObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Array* array = (struct D_Array*)arrayObj;
    evaluator_pushObj(etor, (struct Any*)array_asQueue(array));
}

static void _contains(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Array, T_NULL};
    struct Any* arrayObj;
    struct Any* elem;
    struct Any** paramVars[] = {&arrayObj, &elem};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_Array* array = (struct D_Array*)arrayObj;
    evaluator_pushObj(etor, (struct Any*)boolean_from(array_contains(array, elem)));
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

static void _delete(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Array, T_Integer};
    struct Any* arrayObj;
    struct Any* indexObj;
    struct Any** paramVars[] = {&arrayObj, &indexObj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_Array* array = (struct D_Array*)arrayObj;
    struct D_Integer* index = (struct D_Integer*)indexObj;
    array_delete(array, integer_getValue(index));
    evaluator_pushObj(etor, (struct Any*)array);
}

static void _domain(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Array};
    struct Any* arrayObj;
    struct Any** paramVars[] = {&arrayObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Array* array = (struct D_Array*)arrayObj;
    int count = array_count(array);
    struct D_Sequence* seq = sequence_new(0, count - 1, 1, etor);
    evaluator_pushObj(etor, (struct Any*)seq);
}

static void _insert(struct Evaluator* etor, struct D_List* args) {
    //int nArgs = list_count(args);
    struct Any* arrayObj;
    struct Any* indexObj;
    struct Any* elem;
    struct Any* deadZone = NULL;
    static enum TypeId paramTypes[] = {T_Array, T_Integer, T_NULL, T_NULL};
    struct Any** paramVars[] = {&arrayObj, &indexObj, &elem, &deadZone};
    primitive_checkArgs2(3, 4, paramTypes, args, paramVars, etor);
    struct D_Array* array = (struct D_Array*)arrayObj;
    struct D_Integer* index = (struct D_Integer*)indexObj;
    array_insert(array, integer_getValue(index), elem, deadZone);
    evaluator_pushObj(etor, (struct Any*)array);
}

static void _insertionSort(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Array};
    struct Any* arrayObj;
    struct Any** paramVars[] = {&arrayObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Array* array = (struct D_Array*)arrayObj;
    struct D_Array* newArray = array_insertionSort(array, etor);
    evaluator_pushObj(etor, (struct Any*)newArray);
}

static void _map(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Array, T_NULL};
    struct Any* arrayObj;
    struct Any* abstrObj;
    struct Any** paramVars[] = {&arrayObj, &abstrObj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_Array* array = (struct D_Array*)arrayObj;
    array_map(array, abstrObj, etor);
}

static void _new(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Integer, T_NULL};
    struct Any* countObj;
    struct Any* initialValue = (struct Any*)NIL;
    struct Any** paramVars[] = {&countObj, &initialValue};
    primitive_checkArgs2(1, 2, paramTypes, args, paramVars, etor);
    int count = integer_getValue((struct D_Integer*)countObj);
    struct D_Array* ary = (struct D_Array*)array_newWith(count, initialValue);
    evaluator_pushObj(etor, (struct Any*)ary);
}

void ns_array_get(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Array, T_NULL};
    struct Any* arrayObj;
    struct Any* indexObj;
    struct Any** paramVars[] = {&arrayObj, &indexObj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_Array* array = (struct D_Array*)arrayObj;
    if (T_Integer == any_typeId(indexObj)) {
        struct D_Integer* indexInt = (struct D_Integer*)indexObj;
        int index = integer_getValue(indexInt);
        evaluator_pushObj(etor, array_get(array, index, etor));
    }
    struct Any* val = array_getAssoc(array, indexObj);
    if (val == NULL) {
        struct D_Symbol* sym = symbol_new("Array");
        struct D_Array* exn = array_newN(2, indexObj, (struct Any*)array);
        evaluator_throwException(etor, sym, "key not found in array", (struct Any*)exn);
    }
    evaluator_pushObj(etor, val);
}

static void _reverse(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Array};
    struct Any* arrayObj;
    struct Any** paramVars[] = {&arrayObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Array* array = (struct D_Array*)arrayObj;
    array_reverse(array);
    evaluator_pushObj(etor, (struct Any*)array);
}

static void _set(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Array, T_Integer, T_NULL};
    struct Any* arrayObj;
    struct Any* indexObj;
    struct Any* elem;
    struct Any** paramVars[] = {&arrayObj, &indexObj, &elem};
    primitive_checkArgs(3, paramTypes, args, paramVars, etor);
    struct D_Array* array = (struct D_Array*)arrayObj;
    struct D_Integer* indexInt = (struct D_Integer*)indexObj;
    int index = integer_getValue(indexInt);
    array_set(array, index, elem, etor);
    evaluator_pushObj(etor, (struct Any*)array);
}

static void _selectionSort(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Array};
    struct Any* arrayObj;
    struct Any** paramVars[] = {&arrayObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Array* array = (struct D_Array*)arrayObj;
    array_selectionSort(array, etor);
    evaluator_pushObj(etor, (struct Any*)array);
}

static void _shuffle(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Array};
    struct Any* arrayObj;
    struct Any** paramVars[] = {&arrayObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Array* array = (struct D_Array*)arrayObj;
    array_shuffle(array);
    evaluator_pushObj(etor, (struct Any*)array);
}
