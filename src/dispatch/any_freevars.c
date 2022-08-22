#include <assert.h>

#include "dispatch/methodtable.h"

struct Any;

void any_freeVars(struct Any* obj, struct D_Set* freeVars, struct Evaluator* etor) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    assert(NULL != methods);
    void (*method)(struct Any*, struct D_Set*, struct Evaluator*) = methods->m_freeVars;
    assert(NULL != method);
    method(obj, freeVars, etor);
}
