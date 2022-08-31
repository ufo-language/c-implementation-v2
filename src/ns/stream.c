#include "data/any.h"
#include "data/array.h"
#include "data/boolean.h"
#include "data/hashtable.h"
#include "data/primitive.h"
#include "data/stream.h"
#include "data/string.h"
#include "data/stringbuffer.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "stream"

static void _new(struct Evaluator* etor, struct D_List* args);
static void _readChar(struct Evaluator* etor, struct D_List* args);
static void _writeString(struct Evaluator* etor, struct D_List* args);

void ns_stream_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "new", _new);
    primitive_define(nsHash, "readChar", _readChar);
    primitive_define(nsHash, "writeString", _writeString);
}

static void _new(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String, T_File};
    struct Any* obj = NULL;
    enum TypeId typeId = primitive_checkArgsOneOf(2, paramTypes, args, &obj, etor);
    if (typeId == T_String) {
        struct D_StringBuffer* sb = stringBuffer_new();
        stringBuffer_write((struct D_StringBuffer*)sb, (struct D_String*)obj);
        obj = (struct Any*)sb;
    }
    struct D_Stream* stream = stream_new(obj);
    evaluator_pushObj(etor, (struct Any*)stream);
}

static void _readChar(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Stream};
    struct Any* streamObj;
    struct Any** paramVars[] = {&streamObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_Stream* stream = (struct D_Stream*)streamObj;
    char c;
    bool success = stream_readChar(stream, &c);
    struct Any* res;
    if (success) {
        res = (struct Any*)string_fromChar(c);
    }
    else {
        res = (struct Any*)NIL;
    }
    evaluator_pushObj(etor, res);
}

static void _writeString(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Stream, T_String};
    struct Any* streamObj;
    struct Any* stringObj;
    struct Any** paramVars[] = {&streamObj, &stringObj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_Stream* stream = (struct D_Stream*)streamObj;
    struct D_String* string = (struct D_String*)stringObj;
    bool res = stream_writeString(stream, string);
    evaluator_pushObj(etor, (struct Any*)boolean_from(res));
}
