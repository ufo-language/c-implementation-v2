#include "data/any.h"
#include "data/array.h"
#include "data/binding.h"
#include "data/boolean.h"
#include "data/hashtable.h"
#include "data/list.h"
#include "data/primitive.h"
#include "data/record.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "expr/identifier.h"
#include "main/globals.h"

static void _assert(struct Evaluator* etor, struct D_List* args);
static void _colon(struct Evaluator* etor, struct D_List* args);
static void _equalTo(struct Evaluator* etor, struct D_List* args);
static void _minus(struct Evaluator* etor, struct D_List* args);
static void _percent(struct Evaluator* etor, struct D_List* args);
static void _plus(struct Evaluator* etor, struct D_List* args);
static void _quote(struct Evaluator* etor, struct D_List* args);
static void _reassign(struct Evaluator* etor, struct D_List* args);
static void _sequence(struct Evaluator* etor, struct D_List* args);
static void _slash(struct Evaluator* etor, struct D_List* args);
static void _star(struct Evaluator* etor, struct D_List* args);
static void _throw(struct Evaluator* etor, struct D_List* args);

void globals_percent(struct Any* lhs, struct Any* rhs, struct Evaluator* etor);
void globals_plus(struct Any* lhs, struct Any* rhs, struct Evaluator* etor);
void globals_minus(struct Any* lhs, struct Any* rhs, struct Evaluator* etor);
void globals_reassign(struct Any* lhs, struct Any* rhs, struct Evaluator* etor);
void globals_sequence(struct Any* lhs, struct Any* rhs, struct Evaluator* etor);
void globals_slash(struct Any* lhs, struct Any* rhs, struct Evaluator* etor);
void globals_star(struct Any* lhs, struct Any* rhs, struct Evaluator* etor);

static void _defFun(struct D_HashTable* env, char* name, PrimitiveFunction fun) {
    hashTable_put_unsafe(env, (struct Any*)identifier_new(name),
                         (struct Any*)primitive_new(fun, name, false));
}

static void _defMacro(struct D_HashTable* env, char* name, PrimitiveFunction fun) {
    hashTable_put_unsafe(env, (struct Any*)identifier_new(name),
                         (struct Any*)primitive_new(fun, name, true));
}

void ns_globals_defineAll(struct D_HashTable* env) {
    // functions
    _defFun(env, "==", _equalTo);
    _defFun(env, "+", _plus);
    _defFun(env, "-", _minus);
    _defFun(env, "*", _star);
    _defFun(env, "/", _slash);
    _defFun(env, "%", _percent);
    _defFun(env, "..", _sequence);
    _defFun(env, "assert", _assert);
    _defFun(env, "throw", _throw);
    // macros
    _defMacro(env, ":", _colon);
    _defMacro(env, ":=", _reassign);
    _defMacro(env, "quote", _quote);
}

static void _assert(struct Evaluator* etor, struct D_List* args) {
    struct Any* arg1 = NULL;
    struct Any* arg2 = NULL;
    int nArgs = list_count(args);
    switch(nArgs) {
        case 1:
            arg1 = list_getFirst(args);
            arg2 = (struct Any*)NIL;
            break;
        case 2:
            arg1 = list_getFirst(args);
            arg2 = list_getSecond(args);
            break;
        default:
            primitive_argCountException(1, args, etor);
    }
    if (!any_boolValue(arg1)) {
        struct D_Symbol* argSym = symbol_new("Assertion");
        evaluator_throwException(etor, argSym, "assertion failure", arg2);
    }
    else {
        evaluator_pushObj(etor, (struct Any*)NIL);
    }
}

static void _colon_contin(struct Evaluator* etor, struct Any* arg) {
    struct Any* obj = evaluator_popObj(etor);
    if (any_isA(obj, T_HashTable)) {
        struct D_HashTable* hash = (struct D_HashTable*)obj;
        struct Any* obj1 = hashTable_get(hash, arg, etor);
        evaluator_pushObj(etor, obj1);
    }
    else if (any_isA(obj, T_Record)) {
        struct D_Record* record = (struct D_Record*)obj;
        struct Any* obj1 = record_getFieldValue(record, arg, etor);
        evaluator_pushObj(etor, obj1);
    }
    else {
        evaluator_throwException(
            etor,
            symbol_new("ColonOperator"),
            "colon operator is not defined for object type",
            (struct Any*)array_newN(2, (struct Any*)any_typeSymbol(obj), (struct Any*)obj)
        );
    }
}

static void _colon(struct Evaluator* etor, struct D_List* args) {
    struct Any* first = list_getFirst(args);
    struct Any* second = list_getSecond(args);
    struct E_Continuation* contin = continuation_new(_colon_contin, "colon", second);
    evaluator_pushExpr(etor, (struct Any*)contin);
    evaluator_pushExpr(etor, first);
}

static void _equalTo(struct Evaluator* etor, struct D_List* args) {
    struct Any* lhs = list_getFirst(args);
    struct Any* rhs = list_getSecond(args);
    evaluator_pushObj(etor, (struct Any*)boolean_from(any_isEqual(lhs, rhs)));
}

static void _minus(struct Evaluator* etor, struct D_List* args) {
    struct Any* lhs = list_getFirst(args);
    struct Any* rhs = list_getSecond(args);
    globals_minus(lhs, rhs, etor);
}

static void _percent(struct Evaluator* etor, struct D_List* args) {
    struct Any* lhs = list_getFirst(args);
    struct Any* rhs = list_getSecond(args);
    globals_percent(lhs, rhs, etor);
}

static void _plus(struct Evaluator* etor, struct D_List* args) {
    struct Any* lhs = list_getFirst(args);
    struct Any* rhs = list_getSecond(args);
    globals_plus(lhs, rhs, etor);
}

static void _quote(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_NULL};
    struct Any* expr;
    struct Any** paramVars[] = {&expr};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    evaluator_pushObj(etor, expr);
}

static void _reassignContin(struct Evaluator* etor, struct Any* arg) {
    struct Any* rhs = evaluator_popObj(etor);
    globals_reassign(arg, rhs, etor);
}

static void _reassign(struct Evaluator* etor, struct D_List* args) {
    struct Any* lhs = list_getFirst(args);
    struct Any* rhs = list_getSecond(args);
    evaluator_pushExpr(etor, (struct Any*)continuation_new(_reassignContin, "reassign", lhs));
    evaluator_pushExpr(etor, rhs);
}

static void _sequence(struct Evaluator* etor, struct D_List* args) {
    struct Any* lhs = list_getFirst(args);
    struct Any* rhs = list_getSecond(args);
    globals_sequence(lhs, rhs, etor);
}

static void _slash(struct Evaluator* etor, struct D_List* args) {
    struct Any* lhs = list_getFirst(args);
    struct Any* rhs = list_getSecond(args);
    globals_slash(lhs, rhs, etor);
}

static void _star(struct Evaluator* etor, struct D_List* args) {
    struct Any* lhs = list_getFirst(args);
    struct Any* rhs = list_getSecond(args);
    globals_star(lhs, rhs, etor);
}

static void _throw(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_NULL};
    struct Any* expr;
    struct Any** paramVars[] = {&expr};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    evaluator_throwExceptionObj(etor, expr);
}
