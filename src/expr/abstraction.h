#if !defined(ABSTRACTION_H)
#define ABSTRACTION_H

#include <stdio.h>

struct Any;
struct D_List;
struct D_Set;
struct E_Abstraction;
struct Evaluator;

struct E_Abstraction* abstraction_new(struct D_List* params, struct Any* body);
void abstraction_free(struct E_Abstraction* self);

struct E_Abstraction* abstraction_deepCopy(struct E_Abstraction* self);
void abstraction_eval(struct E_Abstraction* self, struct Evaluator* etor);
void abstraction_freeVars(struct E_Abstraction* self, struct D_Set* freeVars, struct Evaluator* etor);
void abstraction_markChildren(struct E_Abstraction* self);
struct Any* abstraction_getBody(struct E_Abstraction* self);
struct E_Abstraction* abstraction_getNext(struct E_Abstraction* self);
struct D_List* abstraction_getParams(struct E_Abstraction* self);
void abstraction_setNext(struct E_Abstraction* self, struct E_Abstraction* next);
void abstraction_show(struct E_Abstraction* self, FILE* fp);
size_t abstraction_sizeOf(struct E_Abstraction* self);
size_t abstraction_structSize();

#endif
