#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/primitive.h"
#include "data/string.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "version.h"

#define NS_NAME "ufo"

static void _args(struct Evaluator* etor, struct D_List* args);
static void _exitUfo(struct Evaluator* etor, struct D_List* args);
static void _version(struct Evaluator* etor, struct D_List* args);

void ns_ufo_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "args", _args);
    primitive_define(nsHash, "exit", _exitUfo);
    primitive_define(nsHash, "version", _version);
}

extern int ARGC;
extern char** ARGV;

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
    struct Any* exitCodeObj = NULL;
    struct Any** paramVars[] = {&exitCodeObj};
    primitive_checkArgs2(0, 1, paramTypes, args, paramVars, etor);
    if (exitCodeObj == NULL) {
        exitCodeObj = (struct Any*)integer_new(0);
    }
    struct D_Integer* exitCodeInt = (struct D_Integer*)exitCodeObj;
    evaluator_exit(etor, integer_getValue(exitCodeInt));
}

static void _version(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    struct D_String* versionString = string_new(UFO_VERSION);
    evaluator_pushObj(etor, (struct Any*)versionString);
}
