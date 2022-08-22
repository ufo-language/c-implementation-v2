#include "data/any.h"
#include "data/binding.h"
#include "data/hashtable.h"
#include "data/primitive.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "binding"

static void _key(struct Evaluator* etor, struct D_List* args);
static void _new(struct Evaluator* etor, struct D_List* args);
static void _set(struct Evaluator* etor, struct D_List* args);
static void _value(struct Evaluator* etor, struct D_List* args);

void ns_binding_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "key", _key);
    primitive_define(nsHash, "new", _new);
    primitive_define(nsHash, "set", _set);
    primitive_define(nsHash, "value", _value);
}

static void _key(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Binding};
    struct Any* bindingObj;
    struct Any** paramVars[] = {&bindingObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Binding* binding = (struct D_Binding*)bindingObj;
    evaluator_pushObj(etor, (struct Any*)binding_getKey(binding));
}

static void _new(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_NULL, T_NULL};
    struct Any* keyObj;
    struct Any* valueObj;
    struct Any** paramVars[] = {&keyObj, &valueObj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    evaluator_pushObj(etor, (struct Any*)binding_new(keyObj, valueObj));
}

static void _set(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Binding, T_NULL};
    struct Any* bindingObj;
    struct Any* value;
    struct Any** paramVars[] = {&bindingObj, &value};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_Binding* binding = (struct D_Binding*)bindingObj;
    binding_setValue(binding, value);
    evaluator_pushObj(etor, (struct Any*)NIL);
}

static void _value(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Binding};
    struct Any* bindingObj;
    struct Any** paramVars[] = {&bindingObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Binding* binding = (struct D_Binding*)bindingObj;
    evaluator_pushObj(etor, (struct Any*)binding_getValue(binding));
}
