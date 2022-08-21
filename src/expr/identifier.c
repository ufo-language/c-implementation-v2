#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data/any.h"
#include "data/hashtable.h"
#include "data/set.h"
#include "data/string.h"
#include "data/symbol.h"
#include "data/triple.h"
#include "dispatch/methodtable.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "gc/gc.h"
#include "utils/hash.h"

struct E_Identifier {
    struct Any obj;
    char* name;
    unsigned int hashCode;
};

extern struct D_HashTable* INTERNED_IDENTIFIERS;

static unsigned int _hashCode(char* name, int strLen);

struct Methods* identifier_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_compare = (int (*)(struct Any*, struct Any*, struct Evaluator* etor))identifier_compare;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))identifier_eval;
    methods->m_free = (void (*)(struct Any*))identifier_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))identifier_freeVars;
    methods->m_hashCode = (HashCode (*)(struct Any*, struct Evaluator*))identifier_hashCode;
    methods->m_match = (struct D_Triple*(*)(struct Any*, struct Any*, struct D_Triple*))identifier_match;
    methods->m_show = (void (*)(struct Any*, FILE*))identifier_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))identifier_sizeOf;
    methods->m_structSize = identifier_structSize;
    return methods;
}

struct E_Identifier* identifier_new(char* name) {
    struct D_String* nameString = string_new(name);
    struct E_Identifier* self = (struct E_Identifier*)hashTable_get_unsafe(INTERNED_IDENTIFIERS, (struct Any*)nameString);
    if (self == NULL) {
        self = (struct E_Identifier*)gc_alloc(T_Identifier);
        int count = strlen(name);
        self->name = (char*)malloc(count + 1);
        strcpy(self->name, name);
        self->hashCode = _hashCode(name, count);
        hashTable_put_unsafe(INTERNED_IDENTIFIERS, (struct Any*)nameString, (struct Any*)self);
    }
    return self;
}

struct E_Identifier* identifier_new_move(char* name, int count) {
    struct D_String* nameString = string_new(name);
    struct E_Identifier* self = (struct E_Identifier*)hashTable_get_unsafe(INTERNED_IDENTIFIERS, (struct Any*)nameString);
    if (self == NULL) {
        self = (struct E_Identifier*)gc_alloc(T_Identifier);
        self->name = name;
        self->hashCode = _hashCode(name, count);
        hashTable_put_unsafe(INTERNED_IDENTIFIERS, (struct Any*)nameString, (struct Any*)self);
    }
    else {
        free(name);
    }
    return self;
}

void identifier_free(struct E_Identifier* self) {
    free(self->name);
    free(self);
}

int identifier_compare(struct E_Identifier* self, struct E_Identifier* other, struct Evaluator* etor) {
    (void)etor;
    int res = strcmp(self->name, other->name);
    return (res == 0) ? 0 : ((res < 0) ? -1 : 1);
}

void identifier_eval(struct E_Identifier* self, struct Evaluator* etor) {
    struct Any* value = evaluator_lookup(etor, self);
    if (value == NULL) {
        evaluator_throwException(
            etor,
            any_typeSymbol((struct Any*)self),
            "unbound identifier",
            (struct Any*)self
        );
    }
    evaluator_pushObj(etor, value);
}

void identifier_freeVars(struct E_Identifier* self, struct D_Set* freeVars, struct Evaluator* etor) {
    set_add(freeVars, (struct Any*)self, etor);
}

char* identifier_getName(struct E_Identifier* self) {
    return self->name;
}

static unsigned int _hashCode(char* name, int strLen) {
    unsigned int hashCode = 0;
    for (int n=0; n<strLen; n++) {
        hashCode = hashRotateLeft(hashCode) + name[n];
    }
    return hashCode ^ HASH_PRIMES[T_Symbol];
}

unsigned int identifier_hashCode(struct E_Identifier* self, struct Evaluator* etor) {
    (void)etor;
    return self->hashCode;
}

struct D_Triple* identifier_match(struct E_Identifier* self, struct Any* other, struct D_Triple* bindings) {
    return triple_new((struct Any*)self, other, bindings);
}

void identifier_show(struct E_Identifier* self, FILE* fd) {
    fputs(self->name, fd);
}

size_t identifier_sizeOf(struct E_Identifier* self) {
    return sizeof(self);
}

size_t identifier_structSize(enum TypeId typeId) {
    (void)typeId;
    return sizeof(struct E_Identifier);
}
