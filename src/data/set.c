#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/set.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "gc/gc.h"
#include "main/globals.h"

struct D_Set {
    struct Any obj;
    struct D_HashTable* hash;
};

struct D_Set* set_new(void) {
    struct D_Set* self = (struct D_Set*)gc_alloc(T_Set);
    self->hash = hashTable_new();
    return self;
}

void set_free(struct D_Set* self) {
    free(self);
}

void set_add(struct D_Set* self, struct Any* elem, struct Evaluator* etor) {
    hashTable_put(self->hash, elem, elem, etor);
}

void set_addSet(struct D_Set* self, struct D_Set* other, struct Evaluator* etor) {
    struct D_Array* keyAry = set_toArray(other);
    for (int n=0; n<array_count(keyAry); n++) {
        set_add(self, array_get_unsafe(keyAry, n), etor);
    }
}

bool set_boolValue(struct D_Set* self) {
    return hashTable_count(self->hash) > 0;
}

void set_contin(struct Evaluator* etor, struct Any* arg) {
    int count = integer_getValue((struct D_Integer*)arg);
    struct D_Set* set = set_new();
    for (int n=count-1; n>=0; n--) {
        set_add(set, evaluator_popObj(etor), etor);
    }
    evaluator_pushObj(etor, (struct Any*)set);
}

int set_count(struct D_Set* self) {
    return hashTable_count(self->hash);
}

struct D_Set* set_deepCopy(struct D_Set* self) {
    struct D_Set* newSet = set_new();
    // TODO finish
    (void)self;
    fprintf(stderr, "%s is incomplete\n", __func__);
    return newSet;
}

void set_eval(struct D_Set* self, struct Evaluator* etor) {
    struct D_Array* elemAry = set_toArray(self);
    int count = array_count(elemAry);
    evaluator_pushExpr(etor, (struct Any*)continuation_new(set_contin, "set", (struct Any*)integer_new(count)));
    for (int n=0; n<count; n++) {
        evaluator_pushExpr(etor, array_get_unsafe(elemAry, n));
    }
}

void set_freeVars(struct D_Set* self, struct D_Set* freeVars, struct Evaluator* etor) {
    hashTable_freeVars(self->hash, freeVars, etor);
}

bool set_has(struct D_Set* self, struct Any* elem, struct Evaluator* etor) {
    struct Any* elem1 = hashTable_get(self->hash, elem, etor);
    return elem1 != NULL;
}

bool set_isEqual(struct D_Set* self, struct D_Set* other) {
    return hashTable_isEqual(self->hash, other->hash);
}

void set_markChildren(struct D_Set* self) {
    any_mark((struct Any*)self->hash);
}

bool set_removeElem(struct D_Set* self, struct Any* elem, struct Evaluator* etor) {
    return hashTable_remove(self->hash, elem, etor);
}

void set_removeSet(struct D_Set* self, struct D_Set* other, struct Evaluator* etor) {
    struct D_Array* keyAry = set_toArray(other);
    for (int n=0; n<array_count(keyAry); n++) {
        set_removeElem(self, array_get_unsafe(keyAry, n), etor);
    }
}

void set_show(struct D_Set* self, FILE* fp) {
    struct D_Array* keyAry = set_toArray(self);
    array_showWith(keyAry, "${", ", ", "}", fp);
}

size_t set_sizeOf(struct D_Set* self) {
    return sizeof(self);
}

size_t set_structSize(void) {
    return sizeof(struct D_Set);
}

struct D_Array* set_toArray(struct D_Set* self) {
    return hashTable_keyArray(self->hash);
}
