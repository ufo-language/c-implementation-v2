#include "data/any.h"
#include "data/array.h"
#include "data/boolean.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/primitive.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "hash"

void ns_hash_get(struct Evaluator* etor, struct D_List* args);

static void _count(struct Evaluator* etor, struct D_List* args);
static void _hasKey(struct Evaluator* etor, struct D_List* args);
static void _isEmpty(struct Evaluator* etor, struct D_List* args);
static void _keys(struct Evaluator* etor, struct D_List* args);
static void _remove(struct Evaluator* etor, struct D_List* args);
static void _set(struct Evaluator* etor, struct D_List* args);

void ns_hash_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "count", _count);
    primitive_define(nsHash, "get", ns_hash_get);
    primitive_define(nsHash, "hasKey", _hasKey);
    primitive_define(nsHash, "isEmpty", _isEmpty);
    primitive_define(nsHash, "keys", _keys);
    primitive_define(nsHash, "remove", _remove);
    primitive_define(nsHash, "set", _set);
}

static void _count(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_HashTable};
    struct Any* hashObj;
    struct Any** paramVars[] = {&hashObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_HashTable* hash = (struct D_HashTable*)hashObj;
    evaluator_pushObj(etor, (struct Any*)integer_new(hashTable_count(hash)));
}

void ns_hash_get(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_HashTable, T_NULL};
    struct Any* hashObj;
    struct Any* keyObj;
    struct Any** paramVars[] = {&hashObj, &keyObj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_HashTable* hash = (struct D_HashTable*)hashObj;
    struct Any* value = hashTable_get(hash, keyObj);
    if (value == NULL) {
        evaluator_throwException(
            etor,
            symbol_new("HashTable"),
            "key not found in HashTable",
            (struct Any*)array_newN(2, keyObj, hashObj)
        );
    }
    evaluator_pushObj(etor, value);
}

static void _hasKey(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_HashTable, T_NULL};
    struct Any* hashObj;
    struct Any* keyObj;
    struct Any** paramVars[] = {&hashObj, &keyObj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_HashTable* hash = (struct D_HashTable*)hashObj;
    bool res = hashTable_hasKey(hash, keyObj);
    struct D_Boolean* boolObj = boolean_from(res);
    evaluator_pushObj(etor, (struct Any*)boolObj);
}

static void _isEmpty(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_HashTable};
    struct Any* hashObj;
    struct Any** paramVars[] = {&hashObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_HashTable* hash = (struct D_HashTable*)hashObj;
    evaluator_pushObj(etor, (struct Any*)(hashTable_count(hash) == 0 ? TRUE : FALSE));
}

static void _keys(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_HashTable};
    struct Any* hashObj;
    struct Any** paramVars[] = {&hashObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_HashTable* hash = (struct D_HashTable*)hashObj;
    evaluator_pushObj(etor, (struct Any*)hashTable_keyArray(hash));
}

static void _remove(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_HashTable, T_NULL};
    struct Any* hashObj;
    struct Any* keyObj;
    struct Any** paramVars[] = {&hashObj, &keyObj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_HashTable* hash = (struct D_HashTable*)hashObj;
    bool res = hashTable_remove(hash, keyObj);
    evaluator_pushObj(etor, (struct Any*)boolean_from(res));
}

static void _set(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_HashTable, T_NULL, T_NULL};
    struct Any* hashObj;
    struct Any* keyObj;
    struct Any* valueObj;
    struct Any** paramVars[] = {&hashObj, &keyObj, &valueObj};
    primitive_checkArgs(3, paramTypes, args, paramVars, etor);
    struct D_HashTable* hash = (struct D_HashTable*)hashObj;
    hashTable_put(hash, keyObj, valueObj, etor);
    evaluator_pushObj(etor, (struct Any*)NIL);
}
