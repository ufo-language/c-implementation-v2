#if !defined(CONTINUATION_H)
#define CONTINUATION_H

#include <stdio.h>

struct Any;
struct E_Continuation;
struct Evaluator;

typedef void (*ContinuationFunction)(struct Evaluator* etor, struct Any* arg);

struct E_Continuation* continuation_new(ContinuationFunction continFunc, char* name, struct Any* arg);
void continuation_free(struct E_Continuation* self);

void continuation_eval(struct E_Continuation* self, struct Evaluator* etor);
void continuation_markChildren(struct E_Continuation* self);
void continuation_show(struct E_Continuation* self, FILE* fp);
size_t continuation_sizeOf(struct E_Continuation* self);
size_t continuation_structSize(void);

#endif
