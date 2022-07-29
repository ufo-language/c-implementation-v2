#include <dlfcn.h>
#include <stdio.h>

#include "data/address.h"
#include "data/any.h"
#include "data/hashtable.h"
#include "data/list.h"
#include "data/primitive.h"
#include "data/string.h"
#include "data/symbol.h"
#include "expr/identifier.h"
#include "etor/evaluator.h"
#include "main/globals.h"

// see https://tldp.org/HOWTO/Program-Library-HOWTO/dl-libraries.html

#define NS_NAME "ffi"

static void _call(struct Evaluator* etor, struct D_List* args);
static void _close(struct Evaluator* etor, struct D_List* args);
static void _open(struct Evaluator* etor, struct D_List* args);
static void _sym(struct Evaluator* etor, struct D_List* args);

void ns_ffi_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "call", _call);
    primitive_define(nsHash, "close", _close);
    primitive_define(nsHash, "open", _open);
    primitive_define(nsHash, "sym", _sym);
}

static void _call(struct Evaluator* etor, struct D_List* args) {
    (void)args;
    static enum TypeId paramTypes[] = {T_Address};
    struct Any* functionAddress;
    struct Any** paramVars[] = {&functionAddress};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    union FunctionPointer {
        void (*functionCall)();
        void* pointer;
    } functionPointer;
    functionPointer.pointer = address_getValue((struct D_Address*)functionAddress);
    functionPointer.functionCall();
    evaluator_pushObj(etor, (struct Any*)NOTHING);
}

static void _close(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Address};
    struct Any* dlFileAddress;
    struct Any** paramVars[] = {&dlFileAddress};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    void* dlFile = address_getValue((struct D_Address*)dlFileAddress);
    dlclose(dlFile);
    evaluator_pushObj(etor, (struct Any*)NOTHING);
}

static void _open(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String};
    struct Any* fileNameString;
    struct Any** paramVars[] = {&fileNameString};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    char* fileName = string_getChars((struct D_String*)fileNameString);
    void* openRes = dlopen(fileName, RTLD_NOW);
    if (openRes == NULL) {
        evaluator_throwException(
            etor,
            symbol_new("FFI"),
            "Unable to open FFI file",
            (struct Any*)fileNameString
        );
    }
    struct D_Address* address = address_new(openRes);
    evaluator_pushObj(etor, (struct Any*)address);
}

static void _sym(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Address, T_String};
    struct Any* dlFileAddress;
    struct Any* dlFunctionName;
    struct Any** paramVars[] = {&dlFileAddress, &dlFunctionName};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    char* functionName = string_getChars((struct D_String*)dlFunctionName);
    void* dlFile = address_getValue((struct D_Address*)dlFileAddress);
    void* dlFunction = dlsym(dlFile, functionName);
    if (dlFunction == NULL) {
        evaluator_throwException(
            etor,
            symbol_new("FFI"),
            "Unable to locate function name in FFI file",
            (struct Any*)dlFunctionName
        );
        evaluator_pushObj(etor, (struct Any*)NOTHING);
    }
    else {
        struct D_Address* address = address_new(dlFunction);
        evaluator_pushObj(etor, (struct Any*)address);
    }
}
