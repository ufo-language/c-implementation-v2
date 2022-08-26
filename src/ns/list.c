#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/primitive.h"
#include "data/queue.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "expr/apply.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "list"

/*
Object list_accept(Thread* thd, Object args);
Object list_drop(Thread* thd, Object args);
Object list_keys(Thread* thd, Object args);
Object list_map(Thread* thd, Object args);
Object list_reject(Thread* thd, Object args);
Object list_reverse(Thread* thd, Object args);
Object list_setFirst(Thread* thd, Object args);
Object list_setRest(Thread* thd, Object args);
Object list_take(Thread* thd, Object args);
*/

static void _asArray(struct Evaluator* etor, struct D_List* args);
static void _count(struct Evaluator* etor, struct D_List* args);
static void _first(struct Evaluator* etor, struct D_List* args);
void ns_list_get(struct Evaluator* etor, struct D_List* args);
static void _isEmpty(struct Evaluator* etor, struct D_List* args);
static void _iterator(struct Evaluator* etor, struct D_List* args);
static void _map(struct Evaluator* etor, struct D_List* args);
static void _new(struct Evaluator* etor, struct D_List* args);
static void _rest(struct Evaluator* etor, struct D_List* args);

void ns_list_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "asArray", _asArray);
    primitive_define(nsHash, "count", _count);
    primitive_define(nsHash, "first", _first);
    primitive_define(nsHash, "get", ns_list_get);
    primitive_define(nsHash, "isEmpty", _isEmpty);
    primitive_define(nsHash, "iterator", _iterator);
    primitive_define(nsHash, "map", _map);
    primitive_define(nsHash, "new", _new);
    primitive_define(nsHash, "rest", _rest);
}

static void _asArray(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_List};
    struct Any* listObj;
    struct Any** paramVars[] = {&listObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_List* list = (struct D_List*)listObj;
    evaluator_pushObj(etor, (struct Any*)list_asArray(list));
}

static void _count(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_List};
    struct Any* listObj;
    struct Any** paramVars[] = {&listObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_List* list = (struct D_List*)listObj;
    evaluator_pushObj(etor, (struct Any*)integer_new(list_count(list)));
}

static void _first(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_List};
    struct Any* listObj;
    struct Any** paramVars[] = {&listObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_List* list = (struct D_List*)listObj;
    if (list_isEmpty(list)) {
        struct D_Symbol* sym = symbol_new("List");
        evaluator_throwException(etor, sym, "list is empty", (struct Any*)list);
    }
    evaluator_pushObj(etor, (struct Any*)list_getFirst(list));
}

void ns_list_get(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_List, T_NULL};
    struct Any* listObj;
    struct Any* indexObj;
    struct Any** paramVars[] = {&listObj, &indexObj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_List* list = (struct D_List*)listObj;
    if (T_Integer == any_typeId(indexObj)) {
        int index = integer_getValue((struct D_Integer*)indexObj);
        struct Any* elem = list_getNth(list, index, etor);
        evaluator_pushObj(etor, elem);
    }
    else {
        evaluator_throwException(
            etor,
            symbol_new("Get"),
            "list indexing with a non-integer is not yet supported",
            (struct Any*)array_newN(2, indexObj, list)
        );
    }
}

static void _isEmpty(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_List};
    struct Any* listObj;
    struct Any** paramVars[] = {&listObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_List* list = (struct D_List*)listObj;
    evaluator_pushObj(etor, (struct Any*)(list_isEmpty(list) ? TRUE : FALSE));
}

static void _iterator(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_List};
    struct Any* listObj;
    struct Any** paramVars[] = {&listObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_List* list = (struct D_List*)listObj;
    struct D_Iterator* iterator = list_iterator(list);
    evaluator_pushObj(etor, (struct Any*)iterator);
}

// TODO move this into data/list.c (see data/array.c for example)
static void _mapContin(struct Evaluator* etor, struct Any* arg) {
    struct Any* value = evaluator_popObj(etor);
    struct D_Array* argAry = (struct D_Array*)arg;
    struct D_List* elems = (struct D_List*)array_get_unsafe(argAry, 0);
    struct D_Queue* resQ = (struct D_Queue*)array_get_unsafe(argAry, 1);
    queue_enq(resQ, value);
    if (list_isEmpty(elems)) {
        evaluator_pushObj(etor, (struct Any*)queue_asList(resQ));
        return;
    }
    struct Any* nextElem = list_getFirst(elems);
    struct Any* restObj = list_getRest(elems);
    struct Any* abstr = array_get_unsafe(argAry, 2);
    struct D_Array* ary = array_newN(3, restObj, resQ, abstr);
    struct E_Continuation* contin = continuation_new(_mapContin, "list_map", (struct Any*)ary);
    evaluator_pushExpr(etor, (struct Any*)contin);
    struct E_Apply* app = apply_new(abstr, list_new(nextElem, (struct Any*)EMPTY_LIST));
    evaluator_pushExpr(etor, (struct Any*)app);
}

// TODO move this into data/list.c (see data/array.c for example)
static void _map(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_List, T_NULL};
    struct Any* listObj;
    struct Any* abstr;
    struct Any** paramVars[] = {&listObj, &abstr};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_List* list = (struct D_List*)listObj;
    if (list_isEmpty(list)) {
        evaluator_pushObj(etor, (struct Any*)EMPTY_LIST);
    }
    else {
        struct Any* listRest = list_getRest(list);
        struct D_Queue* resQ = queue_new();
        struct D_Array* ary = array_newN(3, listRest, resQ, abstr);
        struct E_Continuation* contin = continuation_new(_mapContin, "list_map", (struct Any*)ary);
        evaluator_pushExpr(etor, (struct Any*)contin);
        struct Any* firstElem = list_getFirst(list);
        struct E_Apply* app = apply_new(abstr, list_new(firstElem, (struct Any*)EMPTY_LIST));
        evaluator_pushExpr(etor, (struct Any*)app);
    }
}

static void _new(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_NULL, T_NULL};
    struct Any* first;
    struct Any* rest;
    struct Any** paramVars[] = {&first, &rest};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    evaluator_pushObj(etor, (struct Any*)list_new(first, rest));
}

static void _rest(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_List};
    struct Any* listObj;
    struct Any** paramVars[] = {&listObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_List* list = (struct D_List*)listObj;
    if (list_isEmpty(list)) {
        struct D_Symbol* sym = symbol_new("List");
        evaluator_throwException(etor, sym, "list is empty", (struct Any*)list);
    }
    evaluator_pushObj(etor, (struct Any*)list_getRest(list));
}
