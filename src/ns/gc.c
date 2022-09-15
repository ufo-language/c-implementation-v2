#include <stdbool.h>

#include "data/any.h"
#include "data/hashtable.h"
#include "data/primitive.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "gc"

extern struct D_HashTable* _globalEnv;

static void _collect(struct Evaluator* etor, struct D_List* args);

void ns_gc_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "collect", _collect);
}

extern bool GC_NEEDED;

static void _collect(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    GC_NEEDED = true;
    evaluator_pushObj(etor, (struct Any*)NIL);
}
