#if !defined(METHODTABLE_H)
#define METHODTABLE_H

#include <stdbool.h>  // for bool
#include <stdio.h>    // for FILE
#include <stdlib.h>   // for size_t

#include "data/any.h"
#include "main/typedefs.h"  // for HashCode

struct D_Iterator;
struct D_Set;
struct D_Triple;
struct Evaluator;

struct Methods {
    bool               (*m_boolValue)(struct Any* self);
    int                (*m_compare)(struct Any* self, struct Any* other, struct Evaluator* etor);
    struct Any*        (*m_deepCopy)(struct Any* self);
    void               (*m_display)(struct Any* self, FILE* fp);
    void               (*m_eval)(struct Any* self, struct Evaluator* etor);
    void               (*m_free)(struct Any* self);
    void               (*m_freeVars)(struct Any* self, struct D_Set* freeVars, struct Evaluator* etor);
    struct Any*        (*m_getPairValue)(struct Any* self, struct Any* key);
    bool               (*m_hashCode)(struct Any* self, HashCode* hashCode);
    bool               (*m_isEqual)(struct Any* self, struct Any* other);
    struct D_Iterator* (*m_iterator)(struct Any* self);
    void               (*m_markChildren)(struct Any* self);
    struct D_Triple*   (*m_match)(struct Any* self, struct Any* other, struct D_Triple* bindings);
    void               (*m_show)(struct Any* self, FILE* fp);
    size_t             (*m_sizeOf)(struct Any* self);
    bool               (*m_streamReadChar)(struct Any* self, char* c);
    bool               (*m_streamWriteChar)(struct Any* self, char c);
    size_t             (*m_structSize)(void);
    struct Any*        (*m_typeOf)(struct Any* self);
    // iterator methods
    bool               (*m_iteratorHasNext)(struct D_Iterator* iter);
    struct Any*        (*m_iteratorNext)(struct D_Iterator* iter);
    // stream methods
};

extern struct Methods* METHOD_TABLE[];

void methodTable_setupDefaults(struct Methods* methods);

#endif
