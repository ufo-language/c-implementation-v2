#include <assert.h>

#include "dispatch/methodtable.h"

struct Any;
struct Evaluator;

int any_compare(struct Any* obj, struct Any* other, struct Evaluator* etor) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    assert(NULL != methods);
    int (*method)(struct Any*, struct Any*, struct Evaluator*) = methods->m_compare;
    assert(NULL != method);
    return method(obj, other, etor);
}
