#include <assert.h>

#include "dispatch/methodtable.h"

struct Any;

bool any_isEqual(struct Any* obj, struct Any* other) {
    if (obj == other) {
        return true;
    }
    if (obj->typeId != other->typeId) {
        return false;
    }
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    assert(NULL != methods);
    bool (*method)(struct Any*, struct Any*) = methods->m_isEqual;
    assert(NULL != method);
    return method(obj, other);
}
