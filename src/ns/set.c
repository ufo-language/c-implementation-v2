#include "data/any.h"
#include "data/array.h"
#include "data/boolean.h"
#include "data/hashtable.h"
#include "data/primitive.h"
#include "data/set.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "set"

static void _add(struct Evaluator* etor, struct D_List* args);
void ns_set_has(struct Evaluator* etor, struct D_List* args);
static void _isEmpty(struct Evaluator* etor, struct D_List* args);
static void _remove(struct Evaluator* etor, struct D_List* args);

void ns_set_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "add", _add);
    primitive_define(nsHash, "has", ns_set_has);
    primitive_define(nsHash, "isEmpty", _isEmpty);
    primitive_define(nsHash, "remove", _remove);
}

static void _add(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Set, T_NULL};
    struct Any* setObj;
    struct Any* elem;
    struct Any** paramVars[] = {&setObj, &elem};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_Set* set = (struct D_Set*)setObj;
    set_add(set, elem, etor);
    evaluator_pushObj(etor, (struct Any*)NIL);
}

void ns_set_has(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Set, T_NULL};
    struct Any* setObj;
    struct Any* elem;
    struct Any** paramVars[] = {&setObj, &elem};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_Set* set = (struct D_Set*)setObj;
    evaluator_pushObj(etor, (struct Any*)boolean_from(set_has(set, elem, etor)));
}

static void _isEmpty(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Set};
    struct Any* setObj;
    struct Any** paramVars[] = {&setObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Set* set = (struct D_Set*)setObj;
    evaluator_pushObj(etor, (struct Any*)boolean_from(0 == set_count(set)));
}

static void _remove(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Set, T_NULL};
    struct Any* setObj;
    struct Any* elem;
    struct Any** paramVars[] = {&setObj, &elem};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_Set* set = (struct D_Set*)setObj;
    evaluator_pushObj(etor, (struct Any*)boolean_from(set_removeElem(set, elem, etor)));
}
