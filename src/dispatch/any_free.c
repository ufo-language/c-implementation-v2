#include <assert.h>

#include "dispatch/methodtable.h"

struct Any;

void any_free(struct Any* obj) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    assert(NULL != methods);
    void (*method)(struct Any*) = methods->m_free;
    assert(NULL != method);
    method(obj);
}
