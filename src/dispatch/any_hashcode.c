#include <assert.h>

#include "dispatch/methodtable.h"

struct Any;

HashCode any_hashCode(struct Any* obj, struct Evaluator* etor) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    assert(NULL != methods);
    HashCode (*method)(struct Any*, struct Evaluator*) = methods->m_hashCode;
    assert(NULL != method);
    return method(obj, etor);
}
