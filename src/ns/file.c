#include <limits.h>

#include "data/any.h"
#include "data/array.h"
#include "data/file.h"
#include "data/hashtable.h"
#include "data/primitive.h"
#include "data/string.h"
#include "data/stringbuffer.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "file"

static void _close(struct Evaluator* etor, struct D_List* args);
static void _isOpen(struct Evaluator* etor, struct D_List* args);
static void _open(struct Evaluator* etor, struct D_List* args);
static void _readAll(struct Evaluator* etor, struct D_List* args);
static void _readChar(struct Evaluator* etor, struct D_List* args);
static void _size(struct Evaluator* etor, struct D_List* args);

void ns_file_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "close", _close);
    primitive_define(nsHash, "isOpen", _isOpen);
    primitive_define(nsHash, "open", _open);
    primitive_define(nsHash, "readAll", _readAll);
    primitive_define(nsHash, "readChar", _readChar);
    primitive_define(nsHash, "size", _size);
}

static void _close(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_File};
    struct Any* fileObj;
    struct Any** paramVars[] = {&fileObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    file_close((struct D_File*)fileObj, etor);
    evaluator_pushObj(etor, (struct Any*)NIL);
}

static void _isOpen(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_File};
    struct Any* fileObj;
    struct Any** paramVars[] = {&fileObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_File* file = (struct D_File*)fileObj;
    evaluator_pushObj(etor, (struct Any*)(file_isOpen(file) ? TRUE : FALSE));
}

static void _open(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String};
    struct Any* fileName;
    struct Any** paramVars[] = {&fileName};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_String* fileNameString = (struct D_String*)fileName;
    struct D_File* file = file_new(fileNameString);
    file_open(file, etor);
    evaluator_pushObj(etor, (struct Any*)file);
}

static void _readChar(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_File};
    struct Any* fileObj;
    struct Any** paramVars[] = {&fileObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_File* file = (struct D_File*)fileObj;
    if (!file_isOpen(file)) {
        struct D_Symbol* sym = symbol_new("File");
        evaluator_throwException(etor, sym, "file is closed", (struct Any*)file);
    }
    int c = file_readChar((struct D_File*)file);
    evaluator_pushObj(etor, (c == EOF) ? (struct Any*)NIL : (struct Any*)string_fromChar(c));
}

static void _readAll(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_File};
    struct Any* fileObj;
    struct Any** paramVars[] = {&fileObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_File* file = (struct D_File*)fileObj;
    if (!file_isOpen(file)) {
        struct D_Symbol* sym = symbol_new("File");
        evaluator_throwException(etor, sym, "file is closed", (struct Any*)file);
    }
    struct D_StringBuffer* sb = stringBuffer_new();
    struct D_Array* sizeAry = file_readAll((struct D_File*)file, sb, etor);
    struct D_Array* resAry = array_newN(2, sizeAry, stringBuffer_asString(sb));
    evaluator_pushObj(etor, (struct Any*)resAry);
}

static void _size(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_File};
    struct Any* fileObj;
    struct Any** paramVars[] = {&fileObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_File* file = (struct D_File*)fileObj;
    long fileSize = file_size(file);
    if (!file_isOpen(file)) {
        struct D_Symbol* sym = symbol_new("File");
        evaluator_throwException(etor, sym, "file is closed", (struct Any*)file);
    }
    int n1 = (int)(fileSize / INT_MAX);
    int n2 = (int)(fileSize % INT_MAX);
    evaluator_pushObj(etor, (struct Any*)array_newN(2, n1, n2));
}
