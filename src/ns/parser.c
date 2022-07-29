#include "data/address.h"
#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/primitive.h"
#include "data/string.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "parser/parser.h"

#define NS_NAME "parser"

static void _parse(struct Evaluator* etor, struct D_List* args);

void ns_parser_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "parse", _parse);
}

static void _parse(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_List};
    struct Any* tokensObj;
    struct Any** paramVars[] = {&tokensObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_List* tokens = (struct D_List*)tokensObj;
    struct Any* expr = parser_parse(&tokens, etor);
    struct D_Array* ary = array_newN(2, expr, tokens);
    evaluator_pushObj(etor, (struct Any*)ary);
}
