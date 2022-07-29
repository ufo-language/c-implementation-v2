#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/primitive.h"
#include "data/string.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"

#define NS_NAME "ufo"

static void _args(struct Evaluator* etor, struct D_List* args);
static void _exitUfo(struct Evaluator* etor, struct D_List* args);

void ns_ufo_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "args", _args);
    primitive_define(nsHash, "exit", _exitUfo);
}

// TODO these two variables are defined in main.c, and I want these
// two variables to be those two variables (in main.c). I should be
// required to put 'extern' in front of both of these variables, but
// that generates an 'undefined reference' compiler error. WHY?
int ARGC;
char** ARGV;

static void _args(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    struct D_Array* argAry = array_new(ARGC);
    for (int n=0; n<ARGC; n++) {
        array_set_unsafe(argAry, n, (struct Any*)string_new(ARGV[n]));
    }
    evaluator_pushObj(etor, (struct Any*)argAry);
}

static void _exitUfo(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Integer};
    struct Any* exitCodeObj;
    struct Any** paramVars[] = {&exitCodeObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Integer* exitCodeInt = (struct D_Integer*)exitCodeObj;
    evaluator_exit(etor, integer_getValue(exitCodeInt));
}
