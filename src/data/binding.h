#if !defined(BINDING_H)
#define BINDING_H

#include <stdbool.h>
#include <stdio.h>

#include "main/typedefs.h"  // for HashCode

struct Any;
struct D_Array;
struct D_Binding;
struct D_Set;
struct D_Triple;
struct Evaluator;

struct D_Binding* binding_new(struct Any* key, struct Any* value);
void binding_initialize(struct D_Binding* self, struct Any* key, struct Any* value);
void binding_free(struct D_Binding* self);

int binding_compare(struct D_Binding* self, struct D_Binding* other, struct Evaluator* etor);
struct D_Binding* binding_deepCopy(struct D_Binding* self);
bool binding_isEqual(struct D_Binding* self, struct D_Binding* other);
void binding_eval(struct D_Binding* self, struct Evaluator* etor);
void binding_freeVars(struct D_Binding* self, struct D_Set* freeVars, struct Evaluator* etor);
struct Any* binding_getKey(struct D_Binding* self);
struct Any* binding_getValue(struct D_Binding* self);
bool binding_hashCode(struct D_Binding* self, HashCode* hashCode);
void binding_markChildren(struct D_Binding* self);
struct D_Triple* binding_match(struct D_Binding* self, struct Any* other, struct D_Triple* bindings);
void binding_setValue(struct D_Binding* self, struct Any* value);
void binding_show(struct D_Binding* self, FILE* fp);
size_t binding_sizeOf(struct D_Binding* self);
size_t binding_structSize(void);

#endif
