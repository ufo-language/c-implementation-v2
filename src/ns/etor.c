#include <stdio.h>

#include "data/address.h"
#include "data/any.h"
#include "data/hashtable.h"
#include "data/list.h"
#include "data/primitive.h"
#include "data/string.h"
#include "expr/identifier.h"
#include "etor/evaluator.h"
#include "main/globals.h"

#define NS_NAME "evaluator"

static void _env(struct Evaluator* etor, struct D_List* args);
static void _eval(struct Evaluator* etor, struct D_List* args);

void ns_etor_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "env", _env);
    primitive_define(nsHash, "eval", _eval);
}

static void _env(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    evaluator_pushObj(etor, (struct Any*)evaluator_getEnv(etor));
}

static void _eval(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_NULL};
    struct Any* expr;
    struct Any** paramVars[] = {&expr};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    evaluator_pushExpr(etor, expr);
}

