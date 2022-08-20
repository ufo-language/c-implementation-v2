#include <stdlib.h>
#include <time.h>

#include "data/any.h"
#include "data/array.h"
#include "data/boolean.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/primitive.h"
#include "data/real.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "rand"

static void _bool(struct Evaluator* etor, struct D_List* args);
static void _choose(struct Evaluator* etor, struct D_List* args);
static void _int(struct Evaluator* etor, struct D_List* args);
static void _max(struct Evaluator* etor, struct D_List* args);
static void _real(struct Evaluator* etor, struct D_List* args);
static void _seed(struct Evaluator* etor, struct D_List* args);

void ns_rand_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "bool", _bool);
    primitive_define(nsHash, "choose", _choose);
    primitive_define(nsHash, "int", _int);
    primitive_define(nsHash, "max", _max);
    primitive_define(nsHash, "real", _real);
    primitive_define(nsHash, "seed", _seed);
}

static void _bool(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    int n = rand() % 2;
    struct D_Boolean* b = boolean_from(n);
    evaluator_pushObj(etor, (struct Any*)b);
}

static void _choose(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Array};
    struct Any* aryObj;
    struct Any** paramVars[] = {&aryObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Array* ary = (struct D_Array*)aryObj;
    int count = array_count(ary);
    struct Any* obj;
    if (count > 0) {
        int n = rand() % count;
        obj = array_get_unsafe(ary, n);
    }
    else {
        obj = (struct Any*)NIL;
    }
    evaluator_pushObj(etor, obj);
}

static void _int(struct Evaluator* etor, struct D_List* args) {
    struct D_Integer* nInt;
    if (list_count(args) == 0) {
        nInt = integer_new(rand());
    }
    else {
        static enum TypeId paramTypes[] = {T_Integer};
        struct Any* intObj;
        struct Any** paramVars[] = {&intObj};
        primitive_checkArgs(1, paramTypes, args, paramVars, etor);
        int limit = integer_getValue((struct D_Integer*)intObj);
        int n = rand() % limit;
        nInt = integer_new(n);
    }
    evaluator_pushObj(etor, (struct Any*)nInt);
}

static void _max(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    evaluator_pushObj(etor, (struct Any*)integer_new(RAND_MAX));
}

static void _real(struct Evaluator* etor, struct D_List* args) {
    struct D_Real* nReal;
    if (list_count(args) == 0) {
        nReal = real_new((double)rand() / RAND_MAX);
    }
    else {
        static enum TypeId paramTypes[] = {T_Real};
        struct Any* realObj;
        struct Any** paramVars[] = {&realObj};
        primitive_checkArgs(1, paramTypes, args, paramVars, etor);
        double limit = real_getValue((struct D_Real*)realObj);
        double n = limit * ((double)rand() / RAND_MAX);
        nReal = real_new(n);
    }
    evaluator_pushObj(etor, (struct Any*)nReal);
}

static void _seed(struct Evaluator* etor, struct D_List* args) {
    if (list_count(args) == 0) {
        srand(time(0));
    }
    else {
        static enum TypeId paramTypes[] = {T_Integer};
        struct Any* intObj;
        struct Any** paramVars[] = {&intObj};
        primitive_checkArgs(1, paramTypes, args, paramVars, etor);
        int seed = integer_getValue((struct D_Integer*)intObj);
        srand(seed);
    }
    evaluator_pushObj(etor, (struct Any*)NIL);
}
