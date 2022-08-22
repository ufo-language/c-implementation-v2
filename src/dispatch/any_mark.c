#include <assert.h>

#include "dispatch/methodtable.h"

struct Any;

void any_mark(struct Any* obj) {
    assert(obj != NULL);
    if (obj->isMarked) {
        return;
    }
    obj->isMarked = true;
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    assert(NULL != methods);
    void (*method)(struct Any*) = methods->m_markChildren;
    assert(NULL != method);
    method(obj);
}
