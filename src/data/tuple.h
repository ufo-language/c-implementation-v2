#if !defined(TUPLE_H)
#define TUPLE_H

#include <stdbool.h>
#include <stdio.h>

#include "main/typedefs.h"

struct Any;
struct D_Set;
struct D_Tuple;
struct Evaluator;

struct D_Tuple* tuple_new(int n);
struct D_Tuple* tuple_newN(int n, ...);
void tuple_free(struct D_Tuple* self);

int tuple_count(struct D_Tuple* self);
struct D_Tuple* tuple_deepCopy(struct D_Tuple* self);
void tuple_eval(struct D_Tuple* self, struct Evaluator* etor);
void tuple_freeVars(struct D_Tuple* self, struct D_Set* freeVars, struct Evaluator* etor);
struct Any* tuple_get_unsafe(struct D_Tuple* self, int n);
//HashCode tuple_hashCode(struct D_Tuple* self, struct Evaluator* etor);
bool tuple_isEqual(struct D_Tuple* self, struct D_Tuple* other);
void tuple_markChildren(struct D_Tuple* self);
struct D_Triple* tuple_match(struct D_Tuple* self, struct Any* other, struct D_Triple* bindings);
void tuple_show(struct D_Tuple* self, FILE* fp);
void tuple_showWith(struct D_Tuple* self, char* open, char* sep, char* close, FILE* fp);
size_t tuple_sizeOf(struct D_Tuple* self);
size_t tuple_structSize(void);

#endif
