#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/primitive.h"
#include "data/set.h"
#include "data/string.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"

#define NS_NAME "any"

static void _deepCopy(struct Evaluator* etor, struct D_List* args);
static void _eval(struct Evaluator* etor, struct D_List* args);
static void _freeVars(struct Evaluator* etor, struct D_List* args);
static void _hashCode(struct Evaluator* etor, struct D_List* args);

void ns_any_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "deepCopy", _deepCopy);
    primitive_define(nsHash, "eval", _eval);
    primitive_define(nsHash, "freeVars", _freeVars);
    primitive_define(nsHash, "hashCode", _hashCode);
}

static void _deepCopy(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_NULL};
    struct Any* obj;
    struct Any** paramVars[] = {&obj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    evaluator_pushObj(etor, any_deepCopy(obj));
}

static void _eval(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_NULL};
    struct Any* expr;
    struct Any** paramVars[] = {&expr};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    evaluator_pushExpr(etor, expr);
}

static void _freeVars(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_NULL};
    struct Any* expr;
    struct Any** paramVars[] = {&expr};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Set* freeVars = set_new();
    any_freeVars(expr, freeVars, etor);
    evaluator_pushObj(etor, (struct Any*)freeVars);
}

static void _hashCode(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_NULL};
    struct Any* expr;
    struct Any** paramVars[] = {&expr};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    HashCode hashCode = any_hashCode(expr, etor);
    evaluator_pushObj(etor, (struct Any*)integer_new(hashCode));
}
