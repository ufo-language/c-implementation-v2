#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/closure.h"
#include "data/list.h"
#include "data/set.h"
#include "etor/evaluator.h"
#include "expr/abstraction.h"
#include "gc/gc.h"
#include "methods/methods.h"

struct E_Abstraction {
    struct Any obj;
    struct D_List* params;
    struct Any* body;
    struct E_Abstraction* next;
};

struct Methods* abstraction_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_deepCopy = (struct Any* (*)(struct Any*))abstraction_deepCopy;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))abstraction_eval;
    methods->m_free = (void (*)(struct Any*))abstraction_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))abstraction_freeVars;
    methods->m_markChildren = (void (*)(struct Any* self))abstraction_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))abstraction_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))abstraction_sizeOf;
    methods->m_structSize = abstraction_structSize;
    return methods;
}

struct E_Abstraction* abstraction_new(struct D_List* params, struct Any* body) {
    struct E_Abstraction* self = (struct E_Abstraction*)gc_alloc(T_Abstraction);
    self->params = params;
    self->body = body;
    self->next = NULL;
    return self;
}

void abstraction_free(struct E_Abstraction* self) {
    free(self);
}

struct E_Abstraction* abstraction_deepCopy(struct E_Abstraction* self) {
    struct E_Abstraction* abstr = abstraction_new(
        list_deepCopy(self->params),
        any_deepCopy(self->body)
    );
    if (self->next != NULL) {
        abstr->next = abstraction_deepCopy(self->next);
    }
    return abstr;
}

void abstraction_eval(struct E_Abstraction* self, struct Evaluator* etor) {
    struct D_Triple* env = evaluator_getEnv(etor);
    struct D_Closure* closure = closure_new(self, env);
    evaluator_pushObj(etor, (struct Any*)closure);
}

void abstraction_freeVars(struct E_Abstraction* self, struct D_Set* freeVars, struct Evaluator* etor) {
    struct D_Set* paramFreeVars = set_new();
    any_freeVars((struct Any*)self->params, paramFreeVars, etor);
    struct D_Set* bodyFreeVars = set_new();
    any_freeVars((struct Any*)self->body, bodyFreeVars, etor);
    set_removeSet(bodyFreeVars, paramFreeVars);
    set_addSet(freeVars, bodyFreeVars, etor);
}

struct Any* abstraction_getBody(struct E_Abstraction* self) {
    return self->body;
}

struct E_Abstraction* abstraction_getNext(struct E_Abstraction* self) {
    return self->next;
}

struct D_List* abstraction_getParams(struct E_Abstraction* self) {
    return self->params;
}

void abstraction_markChildren(struct E_Abstraction* self) {
    any_mark((struct Any*)self->params);
    any_mark((struct Any*)self->body);
    if (self->next != NULL) {
        any_mark((struct Any*)self->next);
    }
}

void abstraction_setNext(struct E_Abstraction* self, struct E_Abstraction* next) {
    self->next = next;
}

void abstraction_show(struct E_Abstraction* self, FILE* fp) {
    list_showWith(self->params, "fun (", ", ", ") = ", fp);
    any_show(self->body, fp);
    struct E_Abstraction* rule = self->next;
    while (rule != NULL) {
        list_showWith(rule->params, "  | (", ", ", ") = ", fp);
        any_show(rule->body, fp);
        rule = rule->next;
    }
}

size_t abstraction_sizeOf(struct E_Abstraction* self) {
    return sizeof(self);
}

size_t abstraction_structSize(void) {
    return sizeof(struct E_Abstraction);
}
