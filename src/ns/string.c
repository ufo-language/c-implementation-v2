#include <string.h>

#include "data/any.h"
#include "data/array.h"
#include "data/boolean.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/primitive.h"
#include "data/queue.h"
#include "data/string.h"
#include "data/stringbuffer.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"
#include "utils/strtok.h"

#define NS_NAME "string"

void ns_string_get(struct Evaluator* etor, struct D_List* args);
static void _count(struct Evaluator* etor, struct D_List* args);
static void _isEmpty(struct Evaluator* etor, struct D_List* args);
static void _iterator(struct Evaluator* etor, struct D_List* args);
static void _join(struct Evaluator* etor, struct D_List* args);
static void _locate(struct Evaluator* etor, struct D_List* args);
static void _split(struct Evaluator* etor, struct D_List* args);
static void _startsWith(struct Evaluator* etor, struct D_List* args);

void ns_string_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "count", _count);
    primitive_define(nsHash, "get", ns_string_get);
    primitive_define(nsHash, "isEmpty", _isEmpty);
    primitive_define(nsHash, "iterator", _iterator);
    primitive_define(nsHash, "join", _join);
    primitive_define(nsHash, "locate", _locate);
    primitive_define(nsHash, "split", _split);
    primitive_define(nsHash, "startsWith", _startsWith);
}

static void _count(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String};
    struct Any* stringObj;
    struct Any** paramVars[] = {&stringObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_String* string = (struct D_String*)stringObj;
    evaluator_pushObj(etor, (struct Any*)integer_new(string_count(string)));
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
    struct D_StringBuffer* sb = stringBuffer_new();
    while (!list_isEmpty(args)) {
        struct Any* arg = list_getFirst(args);
        enum TypeId argTypeId = any_typeId(arg);
        if (T_String != any_typeId(arg)) {
            primitive_argTypeException(T_String, argTypeId, arg, etor);
        }
        struct D_String* string = (struct D_String*)arg;
        stringBuffer_writeChars(sb, string_getChars(string));
        args = (struct D_List*)list_getRestAsList(args);
    }
    evaluator_pushObj(etor, (struct Any*)stringBuffer_asString(sb));
}

static void _locate(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String, T_String, T_Integer};
    struct Any* stringObj;
    struct Any* tokenObj;
    struct Any* indexObj = NULL;
    struct Any** paramVars[] = {&stringObj, &tokenObj, &indexObj};
    primitive_checkArgs2(2, 3, paramTypes, args, paramVars, etor);
    struct D_String* string = (struct D_String*)stringObj;
    struct D_String* token = (struct D_String*)tokenObj;
    int index = 0;
    if (indexObj) {
        index = integer_getValue((struct D_Integer*)indexObj);
    }
    (void)index; // TODO use this
    char* str = string_getChars(string);
    char* tok = string_getChars(token);
    char* res = strstr(str, tok);
    if (res) {
        int diff = res - str;
        evaluator_pushObj(etor, (struct Any*)integer_new(diff));
    }
    else {
        evaluator_pushObj(etor, (struct Any*)NIL);
    }
}

static void _split(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String, T_String};
    struct Any* stringObj;
    struct Any* delimObj;
    struct Any** paramVars[] = {&stringObj, &delimObj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_String* string = (struct D_String*)stringObj;
    struct D_String* delim = (struct D_String*)delimObj;
    struct D_List* tokens = string_split(string, delim);
    evaluator_pushObj(etor, (struct Any*)tokens);
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
