#include <limits.h>  // for INT_MAX
#include <stdio.h>  // for sprintf()

#include "data/any.h"
#include "data/boolean.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/primitive.h"
#include "data/set.h"
#include "data/string.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "symbol"

struct D_HashTable;

extern struct D_HashTable* GENERATED_SYMBOL_NUMBERS;

static void _generate(struct Evaluator* etor, struct D_List* args);

void ns_symbol_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "generate", _generate);
}

static void _generate(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String};
    struct Any* nameObj;
    struct Any** paramVars[] = {&nameObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_String* name = (struct D_String*)nameObj;
    struct D_Integer* symbolNumInt = (struct D_Integer*)hashTable_get(GENERATED_SYMBOL_NUMBERS, (struct Any*)name);
    int symbolNum = 0;
    if (symbolNumInt != NULL) {
        symbolNum = integer_getValue(symbolNumInt);
    }
    struct D_String* completeName = NULL;
    // TODO replace this statically-sized buffer with something dynamic
    char buffer[32];
    for (; symbolNum>0; symbolNum++) {
        sprintf(buffer, "%d", symbolNum);
        struct D_String* numberString = string_new(buffer);
        completeName = string_join(name, numberString);
        struct D_Symbol* symbol = symbol_lookup(completeName);
        if (symbol == NULL) {
            symbol = symbol_new(string_getChars(completeName));
            hashTable_put_unsafe(GENERATED_SYMBOL_NUMBERS, (struct Any*)name, (struct Any*)integer_new(symbolNum + 1));
            evaluator_pushObj(etor, (struct Any*)symbol);
            return;
        }
    }
    struct D_Symbol* sym = symbol_new("Symbol");
    evaluator_throwException(etor, sym, "unable to generate symbol from string", (struct Any*)name);
}
