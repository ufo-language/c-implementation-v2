#if !defined(LOCK_H)
#define LOCK_H

#include <stdio.h>

struct Any;
struct D_Lock;
struct Evaluator;

struct D_Lock* lock_new(void);
void lock_initialize(struct D_Lock* self);
void lock_free(struct D_Lock* self);

void lock_acquire(struct D_Lock* self, struct Evaluator* etor);
void lock_markChildren(struct D_Lock* self);
void lock_release(struct D_Lock* selfg);
void lock_show(struct D_Lock* self, FILE* fp);
size_t lock_sizeOf(struct D_Lock* self);
size_t lock_structSize(void);

#endif
