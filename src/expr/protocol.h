#if !defined(PROTOCOL_H)
#define PROTOCOL_H

#include <stdio.h>

struct Any;
struct D_Set;
struct E_Protocol;
struct Evaluator;
struct Evaluator;

struct E_Protocol* protocol_new(void);
void protocol_free(struct E_Protocol* self);

void protocol_eval(struct E_Protocol* self, struct Evaluator* etor);
void protocol_freeVars(struct E_Protocol* self, struct D_Set* freeVars, struct Evaluator* etor);
void protocol_markChildren(struct E_Protocol* self);
void protocol_show(struct E_Protocol* self, FILE* fp);
size_t protocol_sizeOf(struct E_Protocol* self);
size_t protocol_structSize(void);

#endif

