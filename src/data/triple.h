#if !defined(D_TRIPLE_H)
#define D_TRIPLE_H

#include <stdbool.h>
#include <stdio.h>

struct Any;
struct D_Set;
struct D_Triple;
struct Evaluator;

struct D_Triple* triple_new(struct Any* first, struct Any* second, struct D_Triple* next);
void triple_free(struct D_Triple* self);

bool triple_isEmpty(struct D_Triple* self);
bool triple_isEqual(struct D_Triple* self, struct D_Triple* other);
void triple_freeVars(struct D_Triple* self, struct D_Set* freeVars, struct Evaluator* etor);
struct Any* triple_getFirst(struct D_Triple* self);
struct Any* triple_getSecond(struct D_Triple* self);
struct D_Triple* triple_getNext(struct D_Triple* self);
struct D_Triple* triple_locate(struct D_Triple* self, struct Any* key);
struct Any* triple_lookup(struct D_Triple* self, struct Any* key);
void triple_markChildren(struct D_Triple* self);
struct D_Triple* triple_match(struct D_Triple* self, struct Any* other, struct D_Triple* bindings);
void triple_setSecond(struct D_Triple* self, struct Any* second);
void triple_show(struct D_Triple* self, FILE* fp);
void triple_showWith(struct D_Triple* self, char* open, char* sep, char* close, FILE* fp);
size_t triple_sizeOf(struct D_Triple* self);
size_t triple_structSize();

#endif
