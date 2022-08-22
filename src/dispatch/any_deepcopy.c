#include <assert.h>

#include "dispatch/methodtable.h"

struct Any;

struct Any* any_deepCopy(struct Any* obj) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    assert(NULL != methods);
    struct Any* (*method)(struct Any*) = methods->m_deepCopy;
    assert(NULL != method);
    return method(obj);
}
