#include <assert.h>
#include <string.h>

#include "data/any.h"
#include "data/symbol.h"
#include "utils/hash.h"

char* TYPE_NAMES[] = {
    "NULL",
    "Abstraction",
    "Address",
    "Apply",
    "Array",
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
    "Let",
    "LetIn",
    "LetRec",
    "List",
    "Nil",
    "Primitive",
    "Protocol",
    "Queue",
    "REPL",
    "Real",
    "Record",
    "RecordDefinition",
    "RecordSpec",
    "SavedEnv",
    "Set",
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
