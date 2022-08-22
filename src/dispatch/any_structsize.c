#include <assert.h>

#include "data/any.h"
#include "dispatch/methodtable.h"

struct Any;

size_t any_structSize(enum TypeId typeId) {
    struct Methods* methods = METHOD_TABLE[typeId];
    assert(NULL != methods);
    size_t (*method)(void) = methods->m_structSize;
    assert(NULL != method);
    return method();
}
