#include <assert.h>
#include <stdbool.h>

#include "dispatch/methodtable.h"

struct Any;

bool any_boolValue(struct Any* obj) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    assert(NULL != methods);
    bool (*method)(struct Any*) = methods->m_boolValue;
    assert(NULL != method);
    return method(obj);
}
