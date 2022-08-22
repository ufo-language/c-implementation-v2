#include <assert.h>
#include <stdio.h>

#include "dispatch/methodtable.h"

struct Any;

void any_display(struct Any* obj, FILE* fp) {
    struct Methods* methods = METHOD_TABLE[obj->typeId];
    assert(NULL != methods);
    void (*method)(struct Any*, FILE* fp) = methods->m_display;
    assert(NULL != method);
    method(obj, fp);
}
