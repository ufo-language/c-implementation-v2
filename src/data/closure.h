#if !defined(CLOSURE_H)
#define CLOSURE_H

#include <stdio.h>

struct D_Closure;
struct D_List;
struct D_Set;
struct D_Triple;
struct E_Abstraction;
struct Evaluator;

struct D_Closure* closure_new(struct E_Abstraction* abstr, struct D_Triple* env);
void closure_free(struct D_Closure* self);

void closure_apply(struct D_Closure* self, struct D_List* args, struct Evaluator* etor);
void closure_freeVars(struct D_Closure* self, struct D_Set* freeVars, struct Evaluator* etor);
struct D_List* closure_getParams(struct D_Closure* self);
struct Any* closure_getBody(struct D_Closure* self);
struct D_Triple* closure_getEnv(struct D_Closure* self);
void closure_markChildren(struct D_Closure* self);
void closure_show(struct D_Closure* self, FILE* fp);
size_t closure_sizeOf(struct D_Closure* self);
size_t closure_structSize(void);

#endif
