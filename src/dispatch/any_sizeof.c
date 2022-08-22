#include <assert.h>

#include "dispatch/methodtable.h"

struct Any;

size_t any_sizeOf(struct Any* obj) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    assert(NULL != methods);
    size_t (*method)(struct Any*) = methods->m_sizeOf;
    assert(NULL != method);
    return method(obj);
}
