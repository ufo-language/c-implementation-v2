#include "data/any.h"
#include "data/hashtable.h"
#include "data/primitive.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"

#define NS_NAME "ns"

extern struct D_HashTable* _globalEnv;

static void _global(struct Evaluator* etor, struct D_List* args);

void ns_ns_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "global", _global);
}

static void _global(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    //evaluator_pushObj(etor, (struct Any*)evaluator_getEnv(etor));
    evaluator_pushObj(etor, (struct Any*)_globalEnv);
}
