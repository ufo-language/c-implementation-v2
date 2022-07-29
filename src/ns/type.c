#include "data/any.h"
#include "data/array.h"
#include "data/boolean.h"
#include "data/hashtable.h"
#include "data/primitive.h"
#include "data/set.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "type"

static void _check(struct Evaluator* etor, struct D_List* args);
static void _of(struct Evaluator* etor, struct D_List* args);

void ns_type_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "check", _check);
    primitive_define(nsHash, "of", _of);
}

static void _check(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_NULL, T_Symbol};
    struct Any* obj;
    struct Any* typeSymObj;
    struct Any** paramVars[] = {&obj, &typeSymObj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_Symbol* objTypeSym = any_typeSymbol(obj);
    if (any_isEqual(typeSymObj, (struct Any*)objTypeSym)) {
        evaluator_pushObj(etor, obj);
    }
    else {
        evaluator_throwException(
            etor,
            symbol_new("Type"),
            "type check failure",
            (struct Any*)array_newN(6,
                                    (struct Any*)symbol_new("Expected"),
                                    typeSymObj,
                                    (struct Any*)symbol_new("Found"),
                                    (struct Any*)objTypeSym,
                                    (struct Any*)symbol_new("Object"),
                                    obj)
        );
    }
}

static void _of(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_NULL};
    struct Any* obj;
    struct Any** paramVars[] = {&obj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    evaluator_pushObj(etor, (struct Any*)any_typeOf(obj));
}
