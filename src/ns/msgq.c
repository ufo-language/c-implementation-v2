#include "data/any.h"
#include "data/hashtable.h"
#include "data/msgq.h"
#include "data/primitive.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "msgq"

static void _enq(struct Evaluator* etor, struct D_List* args);
static void _new(struct Evaluator* etor, struct D_List* args);
static void _wait(struct Evaluator* etor, struct D_List* args);

void ns_msgq_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "enq", _enq);
    primitive_define(nsHash, "new", _new);
    primitive_define(nsHash, "wait", _wait);
}

static void _enq(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_MsgQ, T_NULL};
    struct Any* msgqObj;
    struct Any* message;
    struct Any** paramVars[] = {&msgqObj, &message};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_MsgQ* msgq = (struct D_MsgQ*)msgqObj;
    msgq_enq(msgq, message);
    evaluator_pushObj(etor, (struct Any*)NIL);
}

static void _new(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    struct D_MsgQ* msgq = msgq_new();
    evaluator_pushObj(etor, (struct Any*)msgq);
}

static void _wait(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_MsgQ};
    struct Any* msgqObj;
    struct Any** paramVars[] = {&msgqObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_MsgQ* msgq = (struct D_MsgQ*)msgqObj;
    msgq_wait(msgq, etor);
}
