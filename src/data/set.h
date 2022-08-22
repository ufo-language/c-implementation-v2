#if !defined(SET_H)
#define SET_H

#include <stdbool.h>
#include <stdio.h>

#include "any.h"

struct D_Set;
struct D_Triple;
struct Evaluator;

struct D_Set* set_new(void);
void set_free(struct D_Set* self);

void set_add(struct D_Set* self, struct Any* obj, struct Evaluator* etor);
void set_addSet(struct D_Set* self, struct D_Set* other, struct Evaluator* etor);
bool set_boolValue(struct D_Set* self);
int set_count(struct D_Set* self);
struct D_Set* set_deepCopy(struct D_Set* self);
void set_eval(struct D_Set* self, struct Evaluator* etor);
void set_freeVars(struct D_Set* self, struct D_Set* freeVars, struct Evaluator* etor);
bool set_has(struct D_Set* self, struct Any* elem, struct Evaluator* etor);
bool set_isEqual(struct D_Set* self, struct D_Set* other);
void set_markChildren(struct D_Set* self);
//struct D_Triple* set_match(struct D_Set* self, struct Any* other, struct D_Triple* bindings);
bool set_removeElem(struct D_Set* self, struct Any* elem, struct Evaluator* etor);
void set_removeSet(struct D_Set* self, struct D_Set* other, struct Evaluator* etor);
void set_show(struct D_Set* self, FILE* fp);
size_t set_sizeOf(struct D_Set* self);
size_t set_structSize(void);
struct D_Array* set_toArray(struct D_Set* self);

#endif
