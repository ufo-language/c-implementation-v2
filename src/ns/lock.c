#include "data/any.h"
#include "data/hashtable.h"
#include "data/lock.h"
#include "data/primitive.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "lock"

static void _acquire(struct Evaluator* etor, struct D_List* args);
static void _new(struct Evaluator* etor, struct D_List* args);
static void _release(struct Evaluator* etor, struct D_List* args);

void ns_lock_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "acquire", _acquire);
    primitive_define(nsHash, "new", _new);
    primitive_define(nsHash, "release", _release);
}

static void _acquire(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Lock};
    struct Any* lockObj;
    struct Any** paramVars[] = {&lockObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Lock* lock = (struct D_Lock*)lockObj;
    lock_acquire(lock, etor);
    evaluator_pushObj(etor, (struct Any*)NIL);
}

static void _new(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    struct D_Lock* lock = lock_new();
    evaluator_pushObj(etor, (struct Any*)lock);
}

static void _release(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Lock};
    struct Any* lockObj;
    struct Any** paramVars[] = {&lockObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Lock* lock = (struct D_Lock*)lockObj;
    lock_release(lock);
    evaluator_pushObj(etor, (struct Any*)NIL);
}

