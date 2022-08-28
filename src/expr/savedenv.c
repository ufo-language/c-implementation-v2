#include <stdio.h>
#include <stdlib.h>

#include "expr/savedenv.h"
#include "data/any.h"
#include "etor/evaluator.h"
#include "gc/gc.h"
#include "methods/methods.h"

// This is an expression that gets pushed onto the expression stack
// before an application is evaluated (and one or two other
// places). After the application is finished, this expression is
// evaluated and it restores the environment.

struct E_SavedEnv {
    struct Any obj;
    struct D_Triple* env;
};

struct Methods* savedEnv_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))savedEnv_eval;
    methods->m_free = (void (*)(struct Any*))savedEnv_free;
    methods->m_markChildren = (void (*)(struct Any* self))savedEnv_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))savedEnv_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))savedEnv_sizeOf;
    methods->m_structSize = savedEnv_structSize;
    return methods;
}

struct E_SavedEnv* savedEnv_new(struct D_Triple* env) {
    struct E_SavedEnv* self = (struct E_SavedEnv*)gc_alloc(T_SavedEnv);
    self->env = env;
    return self;
}

void savedEnv_free(struct E_SavedEnv* self) {
    free(self);
}

void savedEnv_eval(struct E_SavedEnv* self, struct Evaluator* etor) {
    evaluator_setEnv(etor, self->env);
}

void savedEnv_markChildren(struct E_SavedEnv* self) {
    any_mark((struct Any*)self->env);
}

void savedEnv_show(struct E_SavedEnv* self, FILE* fp) {
    (void)self;
    fprintf(fp, "SavedEnv{}");
}

size_t savedEnv_sizeOf(struct E_SavedEnv* self) {
    return sizeof(self);
}

size_t savedEnv_structSize(void) {
    return sizeof(struct E_SavedEnv);
}
