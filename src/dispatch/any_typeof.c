#include <assert.h>

#include "dispatch/methodtable.h"

struct Any;

struct Any* any_typeOf(struct Any* obj) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    assert(NULL != methods);
    struct Any* (*method)(struct Any* obj) = methods->m_typeOf;
    assert(NULL != method);
    return method(obj);
}
