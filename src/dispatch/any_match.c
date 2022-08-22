#include <assert.h>

#include "dispatch/methodtable.h"

struct Any;
struct D_Triple;

struct D_Triple* any_match(struct Any* obj, struct Any* other, struct D_Triple* bindings) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    assert(NULL != methods);
    struct D_Triple* (*method)(struct Any*, struct Any*, struct D_Triple*) = methods->m_match;
    assert(NULL != method);
    return method(obj, other, bindings);
}
