#include "data/any.h"
#include "data/hashtable.h"
#include "data/primitive.h"
#include "data/string.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "json/json.h"
#include "main/globals.h"

#define NS_NAME "json"

static void _parse(struct Evaluator* etor, struct D_List* args);

void ns_json_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "parse", _parse);
}

static void _parse(struct Evaluator* etor, struct D_List* args) {
    printf("%s got args ", __func__); any_show((struct Any*)args, stdout); printf("\n");
    static enum TypeId paramTypes[] = {T_String};
    struct Any* stringObj;
    struct Any** paramVars[] = {&stringObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_String* string = (struct D_String*)stringObj;
    char* chars = string_getChars(string);
    printf("  chars = '%s'\n", chars);
    json_parse(chars);
    evaluator_pushObj(etor, (struct Any*)NIL);
}
