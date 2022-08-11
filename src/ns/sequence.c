#include "data/address.h"
#include "data/any.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/primitive.h"
#include "data/sequence.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"

#define NS_NAME "sequence"

static void _new(struct Evaluator* etor, struct D_List* args);

void ns_sequence_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "new", _new);
}

static void _new(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Integer, T_Integer};
    struct Any* fromObj;
    struct Any* toObj;
    struct Any** paramVars[] = {&fromObj, &toObj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    int from = integer_getValue((struct D_Integer*)fromObj);
    int to = integer_getValue((struct D_Integer*)toObj);
    struct D_Sequence* sequence = sequence_new(from, to, 1);
    evaluator_pushObj(etor, (struct Any*)sequence);
}
