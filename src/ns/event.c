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

#define NS_NAME "event"

static void _publish(struct Evaluator* etor, struct D_List* args);
static void _subscribe(struct Evaluator* etor, struct D_List* args);
static void _subscribers(struct Evaluator* etor, struct D_List* args);

void ns_event_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "publish", _publish);
    primitive_define(nsHash, "subscribe", _subscribe);
    primitive_define(nsHash, "subscribers", _subscribers);
}

static void _publish(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_NULL};
    struct Any* message;
    struct Any** paramVars[] = {&message};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    // iterate over all the keys and find one that matches the message
    struct D_HashTable* subscriberTable = evaluator_getSubscriberTable(etor);
    if (subscriberTable != NULL) {
        // TODO this is not the most efficient way to do this since it creates a new array
        struct D_Array* keyArray = hashTable_keyArray(subscriberTable);
        for (int n=0; n<array_count(keyArray); n++) {
            struct Any* key = array_get_unsafe(keyArray, n);
            if (any_match(key, message, EMPTY_TRIPLE) != NULL) {
                struct D_Queue* handlerQueue = (struct D_Queue*)hashTable_get(subscriberTable, key);
                struct D_List* args1 = list_new(message, (struct Any*)EMPTY_LIST);
                struct D_List* handlerList = queue_asList(handlerQueue);
                while (!list_isEmpty(handlerList)) {
                    struct Any* handler = list_getFirst(handlerList);
                    struct E_Apply* app = apply_new(handler, args1);
                    evaluator_pushExpr(etor, (struct Any*)app);
                    handlerList = (struct D_List*)list_getRest(handlerList);
                }
                break;
            }
        }
    }
    evaluator_pushObj(etor, (struct Any*)NIL);
}

static void _subscribe(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_NULL, T_NULL};
    struct Any* messagePattern;
    struct Any* subscriber;
    struct Any** paramVars[] = {&messagePattern, &subscriber};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_HashTable* subscriberTable = evaluator_getSubscriberTable(etor);
    if (subscriberTable == NULL) {
        subscriberTable = hashTable_new();
        evaluator_setSubscriberTable(etor, subscriberTable);
    }
    struct D_Queue* subscriberQueue = (struct D_Queue*)hashTable_get(subscriberTable, messagePattern);
    if (subscriberQueue == NULL) {
        subscriberQueue = queue_new();
        hashTable_put(subscriberTable, messagePattern, (struct Any*)subscriberQueue, etor);
    }
    queue_enq(subscriberQueue, subscriber);
    evaluator_pushObj(etor, (struct Any*)NIL);
}

static void _subscribers(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    struct Any* subscribers = (struct Any*)evaluator_getSubscriberTable(etor);
    if (subscribers == NULL) {
        subscribers = (struct Any*)NIL;
    }
    evaluator_pushObj(etor, subscribers);
}
