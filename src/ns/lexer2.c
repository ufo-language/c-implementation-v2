#include "data/address.h"
#include "data/any.h"
#include "data/hashtable.h"
#include "data/primitive.h"
#include "data/string.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "lexer/lexer2.h"

#define NS_NAME "lexer2"

static void _tokenize(struct Evaluator* etor, struct D_List* args);

void ns_lexer2_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "tokenize", _tokenize);
}

static void _tokenize(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String};
    struct Any* stringObj;
    struct Any** paramVars[] = {&stringObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_String* string = (struct D_String*)stringObj;
    struct D_List* tokens = lexer2_tokenize(string);
    evaluator_pushObj(etor, (struct Any*)tokens);
}
