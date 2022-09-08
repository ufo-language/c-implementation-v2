#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data/any.h"
#include "data/hashtable.h"
#include "data/set.h"
#include "data/triple.h"
#include "etor/evaluator.h"
#include "expr/quote.h"
#include "gc/gc.h"
#include "methods/methods.h"

struct E_Quote {
    struct Any obj;
    struct Any* expr;
};

struct Methods* quote_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_compare = (int (*)(struct Any*, struct Any*, struct Evaluator* etor))quote_compare;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))quote_eval;
    methods->m_free = (void (*)(struct Any*))quote_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))quote_freeVars;
    methods->m_show = (void (*)(struct Any*, FILE*))quote_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))quote_sizeOf;
    methods->m_structSize = quote_structSize;
    return methods;
}

struct E_Quote* quote_new(struct Any* expr) {
    struct E_Quote* self = (struct E_Quote*)gc_alloc(T_Quote);
    self->expr = expr;
    return self;
}

void quote_free(struct E_Quote* self) {
    free(self);
}

int quote_compare(struct E_Quote* self, struct E_Quote* other, struct Evaluator* etor) {
    return any_compare(self->expr, other->expr, etor);
}

void quote_eval(struct E_Quote* self, struct Evaluator* etor) {
    evaluator_pushObj(etor, self->expr);
}

void quote_freeVars(struct E_Quote* self, struct D_Set* freeVars, struct Evaluator* etor) {
    any_freeVars(self->expr, freeVars, etor);
}

void quote_show(struct E_Quote* self, FILE* fd) {
    fputc('\'', fd);
    any_show(self->expr, fd);
    fputc('\'', fd);
}

size_t quote_sizeOf(struct E_Quote* self) {
    return sizeof(self);
}

size_t quote_structSize(void) {
    return sizeof(struct E_Quote);
}
