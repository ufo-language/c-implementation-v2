#include "data/any.h"
#include "data/array.h"
#include "data/boolean.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/primitive.h"
#include "data/string.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "string"

void ns_string_get(struct Evaluator* etor, struct D_List* args);
static void _isEmpty(struct Evaluator* etor, struct D_List* args);
static void _iterator(struct Evaluator* etor, struct D_List* args);
static void _join(struct Evaluator* etor, struct D_List* args);
static void _startsWith(struct Evaluator* etor, struct D_List* args);

void ns_string_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "get", ns_string_get);
    primitive_define(nsHash, "isEmpty", _isEmpty);
    primitive_define(nsHash, "iterator", _iterator);
    primitive_define(nsHash, "join", _join);
    primitive_define(nsHash, "startsWith", _startsWith);
}

void ns_string_get(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String, T_Integer};
    struct Any* stringObj;
    struct Any* indexObj;
    struct Any** paramVars[] = {&stringObj, &indexObj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_String* string = (struct D_String*)stringObj;
    int index = integer_getValue((struct D_Integer*)indexObj);
    char c = string_getChar(string, index, etor);
    struct D_String* stringChar = string_fromChar(c);
    evaluator_pushObj(etor, (struct Any*)stringChar);
}

static void _isEmpty(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String};
    struct Any* stringObj;
    struct Any** paramVars[] = {&stringObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_String* string = (struct D_String*)stringObj;
    evaluator_pushObj(etor, (struct Any*)boolean_from(0 == string_count(string)));
}

static void _iterator(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String};
    struct Any* stringObj;
    struct Any** paramVars[] = {&stringObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_String* string = (struct D_String*)stringObj;
    evaluator_pushObj(etor, (struct Any*)string_iterator(string));
}

static void _join(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String, T_String};
    struct Any* string1Obj;
    struct Any* string2Obj;
    struct Any** paramVars[] = {&string1Obj, &string2Obj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_String* string1 = (struct D_String*)string1Obj;
    struct D_String* string2 = (struct D_String*)string2Obj;
    struct D_String* string3 = string_join(string1, string2);
    evaluator_pushObj(etor, (struct Any*)string3);
}

static void _startsWith(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String, T_String};
    struct Any* stringObj;
    struct Any* prefixObj;
    struct Any** paramVars[] = {&stringObj, &prefixObj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_String* string = (struct D_String*)stringObj;
    struct D_String* prefix = (struct D_String*)prefixObj;
    evaluator_pushObj(etor, (struct Any*)boolean_from(string_startsWith(string, prefix)));
}
