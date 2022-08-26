#include <limits.h>

#include "data/any.h"
#include "data/boolean.h"
#include "data/hashtable.h"
#include "data/iterator.h"
#include "data/primitive.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"

#define NS_NAME "iterator"

static void _hasNext(struct Evaluator* etor, struct D_List* args);
static void _next(struct Evaluator* etor, struct D_List* args);

void ns_iterator_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "hasNext", _hasNext);
    primitive_define(nsHash, "next", _next);
}

static void _hasNext(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Iterator};
    struct Any* iteratorObj;
    struct Any** paramVars[] = {&iteratorObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Iterator* iterator = (struct D_Iterator*)iteratorObj;
    evaluator_pushObj(etor, (struct Any*)boolean_from(iterator_boolValue(iterator)));
}

static void _next(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Iterator};
    struct Any* iteratorObj;
    struct Any** paramVars[] = {&iteratorObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Iterator* iterator = (struct D_Iterator*)iteratorObj;
    struct Any* nextObj = iterator_next(iterator);
    if (nextObj == NULL) {
        struct D_Symbol* sym = symbol_new("Iterator");
        evaluator_throwException(etor, sym, "iterator is empty", (struct Any*)iterator);
    }
    else {
        evaluator_pushObj(etor, nextObj);
    }
}
