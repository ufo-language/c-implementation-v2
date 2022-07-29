#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/set.h"
#include "data/symbol.h"
#include "data/term.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "gc/gc.h"

struct D_Term {
    struct Any obj;
    struct D_Symbol* name;
    struct D_Array* elems;
};

struct D_Term* term_new(struct D_Symbol* name, struct D_Array* elems) {
    struct D_Term* self = (struct D_Term*)gc_alloc(T_Term);
    self->name = name;
    self->elems = elems;
    return self;
}

void term_free(struct D_Term* self) {
    free(self);
}

bool term_boolValue(struct D_Term* self) {
    return array_count(self->elems) > 0;
}

int term_count(struct D_Term* self) {
    return array_count(self->elems);
}

static void _contin(struct Evaluator* etor, struct Any* arg) {
    struct D_Array* elems = (struct D_Array*)evaluator_popObj(etor);
    struct D_Term* term = term_new((struct D_Symbol*)arg, elems);
    evaluator_pushObj(etor, (struct Any*)term);
}

struct D_Term* term_deepCopy(struct D_Term* self) {
    return term_new(self->name, array_copy(self->elems));
}

void term_eval(struct D_Term* self, struct Evaluator* etor) {
    evaluator_pushExpr(etor, (struct Any*)continuation_new(_contin, "term", (struct Any*)self->name));
    evaluator_pushExpr(etor, (struct Any*)self->elems);
}

void term_freeVars(struct D_Term* self, struct D_Set* freeVars, struct Evaluator* etor) {
    array_freeVars(self->elems, freeVars, etor);
}

struct Any* term_get(struct D_Term* self, int n) {
    return array_get_unsafe(self->elems, n);
}

bool term_isEqual(struct D_Term* self, struct D_Term* other) {
    return any_isEqual((struct Any*)self->name, (struct Any*)other->name)
        && array_isEqual(self->elems, other->elems);
}

void term_markChildren(struct D_Term* self) {
    any_mark((struct Any*)self->name);
    any_mark((struct Any*)self->elems);
}

struct D_Triple* term_match(struct D_Term* self, struct Any* other, struct D_Triple* bindings) {
    if (!any_isA(other, T_Term)) {
        return NULL;
    }
    struct D_Term* otherTerm = (struct D_Term*)other;
    if (!any_isEqual((struct Any*)self->name, (struct Any*)otherTerm->name)) {
        return NULL;
    }
    return array_match(self->elems, (struct Any*)otherTerm->elems, bindings);
}

struct D_Symbol* term_name(struct D_Term* self) {
    return self->name;
}

void term_set_unsafe(struct D_Term* self, int n, struct Any* elem) {
    array_set_unsafe(self->elems, n, elem);
}

void term_show(struct D_Term* self, FILE* fp) {
    symbol_show(self->name, fp);
    array_show(self->elems, fp);
}

size_t term_sizeOf(struct D_Term* self) {
    (void)self;
    return sizeof(struct D_Term);
}

size_t term_structSize() {
    return sizeof(struct D_Term);
}
