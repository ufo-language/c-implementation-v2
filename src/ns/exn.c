#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/primitive.h"
#include "data/queue.h"
#include "data/symbol.h"
#include "data/unsigned.h"
#include "etor/evaluator.h"
#include "expr/apply.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "exn"

static void _throw(struct Evaluator* etor, struct D_List* args);

void ns_exn_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "throw", _throw);
}

static void _throw(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_NULL};
    struct Any* expr;
    struct Any** paramVars[] = {&expr};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    evaluator_throwExceptionObj(etor, expr);
}
