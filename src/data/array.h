#if !defined(ARRAY_H)
#define ARRAY_H

#include <stdbool.h>
#include <stdio.h>

#include "any.h"

struct D_Array;
struct D_Set;
struct Evaluator;

struct D_Array* array_new(int n);
struct D_Array* array_newN(int n, ...);
void array_free(struct D_Array* self);

int array_compare(struct D_Array* self, struct D_Array* other, struct Evaluator* etor);
struct D_Array* array_copy(struct D_Array* self);
int array_count(struct D_Array* self);
struct D_Array* array_deepCopy(struct D_Array* self);
void array_eval(struct D_Array* self, struct Evaluator* etor);
void array_freeVars(struct D_Array* self, struct D_Set* freeVars, struct Evaluator* etor);
struct Any* array_get_unsafe(struct D_Array* self, int n);
HashCode array_hashCode(struct D_Array* self, struct Evaluator* etor);
bool array_isEqual(struct D_Array* self, struct D_Array* other);
void array_markChildren(struct D_Array* self);
struct D_Triple* array_match(struct D_Array* self, struct Any* other, struct D_Triple* bindings);
void array_set(struct D_Array* self, int n, struct Any* elem, struct Evaluator* etor);
void array_set_unsafe(struct D_Array* self, int n, struct Any* elem);
void array_show(struct D_Array* self, FILE* fp);
void array_showWith(struct D_Array* self, char* open, char* sep, char* close, FILE* fp);
size_t array_sizeOf(struct D_Array* self);
size_t array_structSize(void);

#endif
