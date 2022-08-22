#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/primitive.h"
#include "data/queue.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "queue"

static void _asList(struct Evaluator* etor, struct D_List* args);
static void _count(struct Evaluator* etor, struct D_List* args);
static void _deq(struct Evaluator* etor, struct D_List* args);
static void _enq(struct Evaluator* etor, struct D_List* args);
static void _isEmpty(struct Evaluator* etor, struct D_List* args);
static void _peek(struct Evaluator* etor, struct D_List* args);

void ns_queue_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "asList", _asList);
    primitive_define(nsHash, "count", _count);
    primitive_define(nsHash, "deq", _deq);
    primitive_define(nsHash, "enq", _enq);
    primitive_define(nsHash, "isEmpty", _isEmpty);
    primitive_define(nsHash, "peek", _peek);
}

static void _asList(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Queue};
    struct Any* queueObj;
    struct Any** paramVars[] = {&queueObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Queue* queue = (struct D_Queue*)queueObj;
    evaluator_pushObj(etor, (struct Any*)queue_asList(queue));
}

static void _count(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Queue};
    struct Any* queueObj;
    struct Any** paramVars[] = {&queueObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Queue* queue = (struct D_Queue*)queueObj;
    evaluator_pushObj(etor, (struct Any*)integer_new(queue_count(queue)));
}

static void _deq(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Queue};
    struct Any* queueObj;
    struct Any** paramVars[] = {&queueObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Queue* queue = (struct D_Queue*)queueObj;
    evaluator_pushObj(etor, (struct Any*)queue_deq(queue, etor));
}

static void _enq(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Queue, T_NULL};
    struct Any* queueObj;
    struct Any* elem;
    struct Any** paramVars[] = {&queueObj, &elem};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_Queue* queue = (struct D_Queue*)queueObj;
    queue_enq(queue, elem);
    evaluator_pushObj(etor, (struct Any*)NIL);
}

static void _isEmpty(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Queue};
    struct Any* queueObj;
    struct Any** paramVars[] = {&queueObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Queue* queue = (struct D_Queue*)queueObj;
    evaluator_pushObj(etor, (struct Any*)(queue_isEmpty(queue) ? TRUE : FALSE));
}

static void _peek(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Queue};
    struct Any* queueObj;
    struct Any** paramVars[] = {&queueObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Queue* queue = (struct D_Queue*)queueObj;
    struct Any* obj = queue_peek(queue);
    if (obj == NULL) {
        struct D_Symbol* sym = symbol_new("Queue");
        evaluator_throwException(etor, sym, "queue is empty", (struct Any*)queue);
    }
    evaluator_pushObj(etor, obj);
}
