#if !defined(GC_H)
#define GC_H

#include <stdbool.h>

#include "data/any.h"

struct Any* gc_alloc(enum TypeId typeId);
void gc_collect(void);
void gc_commit(void);
void gc_commitPermanentObjects(void);
void gc_freeBytes(size_t bytes);
void gc_markRoots(void);
void gc_register(struct Any* obj);
void gc_showStats(void);
void gc_start(void);
void gc_startFunctions(void (*setupPermanentObjects)(void),
                       void (*setupRootObjects)(void),
                       void (*initialize)(void));
void gc_stop(void);
void gc_sweep(void);
void gc_transferToRoots(struct Any* obj);

// debugging functions
void gc_dump(void);
bool gc_isCommitted(struct Any* obj);
bool gc_isPermanent(struct Any* obj);
bool gc_isRoot(struct Any* obj);
bool gc_isSpine(struct Any* obj);

extern bool GC_NEEDED;

#endif
