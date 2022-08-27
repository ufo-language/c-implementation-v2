#include <limits.h>

#include "data/any.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/primitive.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"

#define NS_NAME "integer"

static void _iterator(struct Evaluator* etor, struct D_List* args);
static void _max(struct Evaluator* etor, struct D_List* args);
static void _min(struct Evaluator* etor, struct D_List* args);

void ns_integer_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "iterator", _iterator);
    primitive_define(nsHash, "max", _max);
    primitive_define(nsHash, "min", _min);
}

static void _iterator(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Integer};
    struct Any* intObj;
    struct Any** paramVars[] = {&intObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Integer* integer = (struct D_Integer*)intObj;
    struct D_Iterator* iterator = integer_iterator(integer);
    evaluator_pushObj(etor, (struct Any*)iterator);
}

static void _max(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    evaluator_pushObj(etor, (struct Any*)integer_new(INT_MAX));
}

static void _min(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    evaluator_pushObj(etor, (struct Any*)integer_new(INT_MIN));
}
