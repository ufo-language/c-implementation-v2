#include <stdio.h>
#include <stdlib.h>

#include "expr/continuation.h"
#include "data/any.h"
#include "dispatch/methodtable.h"
#include "etor/evaluator.h"
#include "gc/gc.h"
#include "main/globals.h"

struct E_Continuation {
    struct Any obj;
    ContinuationFunction continFunc;
    char* name;
    struct Any* arg;
};

struct Methods* continuation_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    //methods->m_deepCopy = (struct Any* (*)(struct Any*))continuation_deepCopy;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))continuation_eval;
    methods->m_free = (void (*)(struct Any*))continuation_free;
    methods->m_markChildren = (void (*)(struct Any* self))continuation_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))continuation_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))continuation_sizeOf;
    methods->m_structSize = continuation_structSize;
    return methods;
}

struct E_Continuation* continuation_new(ContinuationFunction continFunc, char* name, struct Any* arg) {
    struct E_Continuation* self = (struct E_Continuation*)gc_alloc(T_Continuation);
    self->continFunc = continFunc;
    self->name = name;
    self->arg = arg;
    return self;
}

void continuation_free(struct E_Continuation* self) {
    free(self);
}

void continuation_eval(struct E_Continuation* self, struct Evaluator* etor) {
    self->continFunc(etor, self->arg);
}

void continuation_markChildren(struct E_Continuation* self) {
    any_mark(self->arg);
}

void continuation_show(struct E_Continuation* self, FILE* fp) {
    fprintf(fp, "Continuation{%s", self->name);
    if (self->arg != (struct Any*)NIL) {
        fputs(", ", fp);
        any_show(self->arg, fp);
    }
    fputc('}', fp);
}

size_t continuation_sizeOf(struct E_Continuation* self) {
    return sizeof(self);
}

size_t continuation_structSize(void) {
    return sizeof(struct E_Continuation);
}
