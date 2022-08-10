#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/primitive.h"
#include "data/string.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "gc/gc.h"
#include "main/typedefs.h"
#include "utils/hash.h"

struct D_Primitive {
    struct Any obj;
    char* name;
    bool isMacro;
    PrimitiveFunction primFunc;
};

union FunctionPointerUnion {
    PrimitiveFunction function;
    //void* pointer;
    HashCode hashCode;
};

void primitive_argCountException(int nParams, struct D_List* argList, struct Evaluator* etor) {
    struct D_Array* exn = array_newN(6,
                                     symbol_new("Expected"),
                                     integer_new(nParams),
                                     symbol_new("Found"),
                                     integer_new(list_count(argList)),
                                     symbol_new("ArgumentList"),
                                     argList);
    struct D_Symbol* argSym = symbol_new("Argument");
    evaluator_throwException(etor, argSym, "incorrect number of arguments", (struct Any*)exn);
}

void primitive_argTypeException(enum TypeId expectedTypeId, enum TypeId foundTypeId, struct Any* argument, struct Evaluator* etor) {
    struct D_Array* exn = array_newN(5,
                                     symbol_new("Expected"),
                                     symbol_new(TYPE_NAMES[expectedTypeId]),
                                     symbol_new("Found"),
                                     symbol_new(TYPE_NAMES[foundTypeId]),
                                     argument);
    struct D_Symbol* argSym = symbol_new("Argument");
    evaluator_throwException(etor, argSym, "type mismatch", (struct Any*)exn);
}

void primitive_checkArgs(int nParams, enum TypeId paramTypes[], struct D_List* argList, struct Any** paramVars[], struct Evaluator* etor) {
    struct D_List* savedArgList = argList;
    for (int n=0; n<nParams; n++) {
        if (list_isEmpty(argList)) {
            primitive_argCountException(nParams, savedArgList, etor);
        }
        struct Any* arg = list_getFirst(argList);
        argList = (struct D_List*)list_getRest(argList);
        if (paramTypes[n] != T_NULL && paramTypes[n] != any_typeId(arg)) {
            primitive_argTypeException(paramTypes[n], any_typeId(arg), arg, etor);
        }
        *paramVars[n] = arg;
    }
    if (!list_isEmpty(argList)){
        primitive_argCountException(nParams, savedArgList, etor);
    }
}

void primitive_checkArgs2(int nParams1, int nParams2, enum TypeId paramTypes[], struct D_List* argList, struct Any** paramVars[], struct Evaluator* etor) {
    struct D_List* savedArgList = argList;
    for (int n=0; n<nParams2; n++) {
        if (list_isEmpty(argList)) {
            if (n < nParams1) {
                primitive_argCountException(nParams1, savedArgList, etor);
            }
            break;
        }
        struct Any* arg = list_getFirst(argList);
        argList = (struct D_List*)list_getRest(argList);
        if (paramTypes[n] != T_NULL && paramTypes[n] != any_typeId(arg)) {
            primitive_argTypeException(paramTypes[n], any_typeId(arg), arg, etor);
        }
        *paramVars[n] = arg;
    }
    if (!list_isEmpty(argList)){
        primitive_argCountException(nParams2, savedArgList, etor);
    }
}

void primitive_define(struct D_HashTable* namespace, char* name, PrimitiveFunction prim) {
    hashTable_put_unsafe(
        namespace,
        (struct Any*)identifier_new(name),
        (struct Any*)primitive_new(prim, name, false)
    );
}

void primitive_defineMacro(struct D_HashTable* namespace, char* name, PrimitiveFunction prim) {
    hashTable_put_unsafe(
        namespace,
        (struct Any*)identifier_new(name),
        (struct Any*)primitive_new(prim, name, true)
    );
}

struct D_Primitive* primitive_new(PrimitiveFunction primFunc, char* name, bool isMacro) {
    struct D_Primitive* self = (struct D_Primitive*)gc_alloc(T_Primitive);
    self->name = name;
    self->isMacro = isMacro;
    self->primFunc = primFunc;
    return self;
}

void primitive_free(struct D_Primitive* self) {
    free(self);
}

void primitive_apply(struct D_Primitive* self, struct D_List* args, struct Evaluator* etor) {
    self->primFunc(etor, args);
}

HashCode primitive_hashCode(struct D_Primitive* self, struct Evaluator* etor) {
    (void)etor;
    union FunctionPointerUnion fpu;
    fpu.function = self->primFunc;
    return fpu.hashCode ^ HASH_PRIMES[T_Primitive];
}

bool primitive_isMacro(struct D_Primitive* self) {
    return self->isMacro;
}

void primitive_show(struct D_Primitive* self, FILE* fp) {
#if 0
    union FunctionPointerUnion fpu;
    fpu.function = self->primFunc;
    fprintf(fp, "Primitive{%s, %p}", self->name, fpu.pointer);
#else
    fprintf(fp, "Primitive{%s}", self->name);
#endif
}

size_t primitive_sizeOf(struct D_Primitive* self) {
    return sizeof(self);
}

size_t primitive_structSize() {
    return sizeof(struct D_Primitive);
}
