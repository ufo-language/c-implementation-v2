#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data/any.h"
#include "data/hashtable.h"
#include "data/string.h"
#include "data/symbol.h"
#include "dispatch/methodtable.h"
#include "gc/gc.h"
#include "main/typedefs.h"  // for HashCode
#include "utils/hash.h"

extern struct D_HashTable* INTERNED_SYMBOLS;

struct D_Symbol {
    struct Any obj;
    char* name;
    HashCode hashCode;
};

static HashCode _hashCode(char* name, int strLen);

struct Methods* symbol_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_compare = (int (*)(struct Any*, struct Any*, struct Evaluator* etor))symbol_compare;
    methods->m_free = (void (*)(struct Any*))symbol_free;
    methods->m_hashCode = (bool (*)(struct Any*, HashCode*))symbol_hashCode;
    methods->m_show = (void (*)(struct Any*, FILE*))symbol_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))symbol_sizeOf;
    methods->m_structSize = symbol_structSize;
    return methods;
}

struct D_Symbol* symbol_new(char* name) {
    struct D_String* nameString = string_new(name);
    struct D_Symbol* self = (struct D_Symbol*)hashTable_get(INTERNED_SYMBOLS, (struct Any*)nameString);
    if (self == NULL) {    
        self = (struct D_Symbol*)gc_alloc(T_Symbol);
        int count = strlen(name);
        self->name = (char*)malloc(count + 1);
        strcpy(self->name, name);
        self->hashCode = _hashCode(name, count);
        hashTable_put_unsafe(INTERNED_SYMBOLS, (struct Any*)nameString, (struct Any*)self);
    }
    return self;
}

struct D_Symbol* symbol_new_move(char* name, int count) {
    struct D_String* nameString = string_new(name);
    struct D_Symbol* self = (struct D_Symbol*)hashTable_get(INTERNED_SYMBOLS, (struct Any*)nameString);
    if (self == NULL) {    
        self = (struct D_Symbol*)gc_alloc(T_Symbol);
        self->name = name;
        self->hashCode = _hashCode(name, count);
        hashTable_put_unsafe(INTERNED_SYMBOLS, (struct Any*)nameString, (struct Any*)self);
    }
    else {
        free(name);
    }
    return self;
}

void symbol_free(struct D_Symbol* self) {
    free(self->name);
    free(self);
}

int symbol_compare(struct D_Symbol* self, struct D_Symbol* other, struct Evaluator* etor) {
    (void)etor;
    int res = strcmp(self->name, other->name);
    return (res == 0) ? 0 : ((res < 0) ? -1 : 1);
}

char* symbol_getName(struct D_Symbol* self) {
    return self->name;
}

static HashCode _hashCode(char* name, int strLen) {
    HashCode hashCode = 0;
    for (int n=0; n<strLen; n++) {
        hashCode = hashRotateLeft(hashCode) + name[n];
    }
    return hashCode ^ HASH_PRIMES[T_Symbol];
}

bool symbol_hashCode(struct D_Symbol* self, HashCode* hashCode) {
    *hashCode = self->hashCode;
    return true;
}

void symbol_show(struct D_Symbol* self, FILE* fd) {
    fputs(self->name, fd);
}

size_t symbol_sizeOf(struct D_Symbol* self) {
    (void)self;
    return sizeof(struct D_Symbol);
}

size_t symbol_structSize(void) {
    return sizeof(struct D_Symbol);
}
