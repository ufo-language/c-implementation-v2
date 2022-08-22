#include <stdio.h>
#include <stdlib.h>

#include "data/binding.h"
#include "data/any.h"
#include "data/array.h"
#include "data/set.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "main/typedefs.h"
#include "utils/hash.h"

struct D_Binding {
    struct Any obj;
    struct Any* key;
    struct Any* value;
};

struct D_Binding* binding_new(struct Any* key, struct Any* value) {
    struct D_Binding* self = (struct D_Binding*)gc_alloc(T_Binding);
    binding_initialize(self, key, value);
    return self;
}

void binding_initialize(struct D_Binding* self, struct Any* key, struct Any* value) {
    self->key = key;
    self->value = value;
}

void binding_free(struct D_Binding* self) {
    free(self);
}

int binding_compare(struct D_Binding* self, struct D_Binding* other, struct Evaluator* etor) {
    int res = any_compare(self->key, other->key, etor);
    if (res == 0) {
        res = any_compare(self->value, other->value, etor);
    }
    return res;
}

void binding_contin(struct Evaluator* etor, struct Any* arg) {
    (void)arg;
    struct Any* value = evaluator_popObj(etor);
    struct Any* key = evaluator_popObj(etor);
    struct D_Binding* binding = binding_new(key, value);
    evaluator_pushObj(etor, (struct Any*)binding);
}

struct D_Binding* binding_deepCopy(struct D_Binding* self) {
    return binding_new(any_deepCopy(self->key), any_deepCopy(self->value));
}

void binding_eval(struct D_Binding* self, struct Evaluator* etor) {
    evaluator_pushExpr(etor, (struct Any*)continuation_new(binding_contin, "binding", (struct Any*)NIL));
    evaluator_pushExpr(etor, self->value);
    evaluator_pushExpr(etor, self->key);
}

bool binding_isEqual(struct D_Binding* self, struct D_Binding* other) {
    return any_isEqual(self->key, other->key) && any_isEqual(self->value, other->value);
}

void binding_freeVars(struct D_Binding* self, struct D_Set* freeVars, struct Evaluator* etor) {
    any_freeVars(self->key, freeVars, etor);
    any_freeVars(self->value, freeVars, etor);
}

struct Any* binding_getKey(struct D_Binding* self) {
    return self->key;
}

struct Any* binding_getValue(struct D_Binding* self) {
    return self->value;
}

HashCode binding_hashCode(struct D_Binding* self, struct Evaluator* etor) {
    HashCode hashCode = any_hashCode(self->key, etor);
    hashCode = hashRotateLeft(hashCode) ^ any_hashCode(self->value, etor);
    return hashCode ^ HASH_PRIMES[T_Binding];
}

void binding_markChildren(struct D_Binding* self) {
    any_mark(self->key);
    any_mark(self->value);
}

struct D_Triple* binding_match(struct D_Binding* self, struct Any* other, struct D_Triple* bindings) {
    if (!any_isA(other, T_Binding)) {
        return NULL;
    }
    struct D_Binding* binding = (struct D_Binding*)other;
    bindings = any_match(self->key, binding->key, bindings);
    if (bindings == NULL) {
        return NULL;
    }
    return any_match(self->value, binding->value, bindings);
}

void binding_setValue(struct D_Binding* self, struct Any* value) {
    self->value = value;
}

void binding_show(struct D_Binding* self, FILE* fp) {
    if (any_isA(self->key, T_Binding)) {
        fputc('(', fp);
        any_show(self->key, fp);
        fputc(')', fp);
    }
    else {
        any_show(self->key, fp);
    }
    fputc('=', fp);
    if (any_isA(self->value, T_Binding)) {
        fputc('(', fp);
        any_show(self->value, fp);
        fputc(')', fp);
    }
    else {
        any_show(self->value, fp);
    }
}

size_t binding_sizeOf(struct D_Binding* self) {
    return sizeof(self);
}

size_t binding_structSize(void) {
    return sizeof(struct D_Binding);
}
