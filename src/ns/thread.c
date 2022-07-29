#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/primitive.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "thread"

static void _future(struct Evaluator* etor, struct D_List* args);
static void _self(struct Evaluator* etor, struct D_List* args);
static void _spawn(struct Evaluator* etor, struct D_List* args);

void ns_thread_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "future", _future);
    primitive_define(nsHash, "self", _self);
    primitive_define(nsHash, "spawn", _spawn);
}

static void _future(struct Evaluator* etor, struct D_List* args) {
    /*
    static enum TypeId paramTypes[] = {T_Thread};
    struct Any* threadObj;
    struct Any** paramVars[] = {&threadObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Thread* thread = (struct D_Thread*)threadObj;
    evaluator_pushObj(etor, (struct Any*)thread_asList(thread));
    */
    (void)args;
    printf("%s is incomplete\n", __func__);
    evaluator_pushObj(etor, (struct Any*)NOTHING);
}

static void _self(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    evaluator_pushObj(etor, (struct Any*)etor);
}

// This creates a new evaluator instance and returns it.
static void _spawn(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_NULL};
    struct Any* expr;
    struct Any** paramVars[] = {&expr};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct Evaluator* etorNew = evaluator_new();
    evaluator_pushExpr(etorNew, expr);
    evaluator_pushObj(etor, (struct Any*)etorNew);
}
