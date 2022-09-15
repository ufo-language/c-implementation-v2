#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "data/any.h"
#include "data/symbol.h"
#include "methods/methods.h"
#include "utils/hash.h"

struct Evaluator;
struct D_Triple;

char* TYPE_NAMES[] = {
    "NULL",
    "Abstraction",
    "Address",
    "Apply",
    "Array",
    "Async",
    "BinOp",
    "Binding",
    "Boolean",
    "BracketExpr",
    "Closure",
    "Cobegin",
    "Continuation",
    "Do",
    "Evaluator",
    "ExceptionHandler",
    "File",
    "HashTable",
    "Identifier",
    "If",
    "Integer",
    "Iterator",
    "Let",
    "LetIn",
    "LetRec",
    "Lock",
    "Loop",
    "List",
    "MsgQ",
    "Nil",
    "Nondet",
    "Primitive",
    "Protocol",
    "Queue",
    "Quote",
    "REPL",
    "Real",
    "Record",
    "RecordDefinition",
    "RecordSpec",
    "SavedEnv",
    "Sequence",
    "Set",
    "Stream",
    "String",
    "StringBuffer",
    "StringStream",
    "Symbol",
    "Term",
    "Triple",
    "TryCatch",
    "Tuple",
    "Unsigned",
    "FINAL"
};

// Direct-call methods ===============================================

void any_sanityCheck(void) {
    assert(0 == strcmp("FINAL", TYPE_NAMES[T_FINAL]));
    assert(0 != HASH_PRIMES[T_FINAL]);
}

bool any_hasType(struct Any* obj, struct Any* typeObj) {
    if (any_isA(typeObj, T_Symbol)) {
        return any_isEqual((struct Any*)any_typeSymbol(obj), typeObj);
    }
    return false;
}

bool any_isA(struct Any* obj, enum TypeId typeId) {
    return obj->typeId == typeId;
}

bool any_isMarked(struct Any* obj) {
    return obj->isMarked;
}

enum TypeId any_typeId(struct Any* obj) {
    return obj->typeId;
}

char* any_typeName(struct Any* obj) {
    return TYPE_NAMES[obj->typeId];
}

struct D_Symbol* any_typeSymbol(struct Any* obj) {
    enum TypeId typeId = obj->typeId;
    char* typeName = TYPE_NAMES[typeId];
    return symbol_new(typeName);
}

// Dispatch methods ==================================================

bool any_boolValue(struct Any* obj) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    bool (*method)(struct Any*) = methods->m_boolValue;
    return method(obj);
}

int any_compare(struct Any* obj, struct Any* other, struct Evaluator* etor) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    int (*method)(struct Any*, struct Any*, struct Evaluator*) = methods->m_compare;
    return method(obj, other, etor);
}

struct Any* any_deepCopy(struct Any* obj) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    struct Any* (*method)(struct Any*) = methods->m_deepCopy;
    return method(obj);
}

void any_display(struct Any* obj, FILE* fp) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    void (*method)(struct Any*, FILE* fp) = methods->m_display;
    method(obj, fp);
}

void any_eval(struct Any* obj, struct Evaluator* etor) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    void (*method)(struct Any*, struct Evaluator*) = methods->m_eval;
    method(obj, etor);
}

void any_free(struct Any* obj) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    void (*method)(struct Any*) = methods->m_free;
    method(obj);
}

void any_freeVars(struct Any* obj, struct D_Set* freeVars, struct Evaluator* etor) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    void (*method)(struct Any*, struct D_Set*, struct Evaluator*) = methods->m_freeVars;
    method(obj, freeVars, etor);
}

struct Any* any_getPairValue(struct Any* obj, struct Any* key) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    struct Any* (*method)(struct Any*, struct Any*) = methods->m_getPairValue;
    return method(obj, key);
}

bool any_hashCode(struct Any* obj, HashCode* hashCode) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    bool (*method)(struct Any*, HashCode*) = methods->m_hashCode;
    return method(obj, hashCode);
}

bool any_isEqual(struct Any* obj, struct Any* other) {
    if (obj == other) {
        return true;
    }
    if (obj->typeId != other->typeId) {
        return false;
    }
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    bool (*method)(struct Any*, struct Any*) = methods->m_isEqual;
    return method(obj, other);
}

struct D_Iterator* any_iterator(struct Any* obj) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    struct D_Iterator* (*method)(struct Any*) = methods->m_iterator;
    return method(obj);
}

void any_mark(struct Any* obj) {
    if (obj->isMarked) {
        return;
    }
    obj->isMarked = true;
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    void (*method)(struct Any*) = methods->m_markChildren;
    method(obj);
}

struct D_Triple* any_match(struct Any* obj, struct Any* other, struct D_Triple* bindings) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    struct D_Triple* (*method)(struct Any*, struct Any*, struct D_Triple*) = methods->m_match;
    return method(obj, other, bindings);
}

void any_show(struct Any* obj, FILE* fp) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    void (*method)(struct Any*, FILE* fp) = methods->m_show;
    method(obj, fp);
}

size_t any_sizeOf(struct Any* obj) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    size_t (*method)(struct Any*) = methods->m_sizeOf;
    return method(obj);
}

size_t any_structSize(enum TypeId typeId) {
    struct Methods* methods = METHOD_TABLE[typeId];
    size_t (*method)(void) = methods->m_structSize;
    return method();
}

struct Any* any_typeOf(struct Any* obj) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    struct Any* (*method)(struct Any* obj) = methods->m_typeOf;
    return method(obj);
}
