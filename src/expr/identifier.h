#if !defined(IDENTIFIER_H)
#define IDENTIFIER_H

#include <stdbool.h>
#include <stdio.h>

#include "main/typedefs.h"

struct Any;
struct D_Set;
struct D_Triple;
struct Evaluator;
struct Evaluator;
struct E_Identifier;

struct E_Identifier* identifier_new(char* name);
struct E_Identifier* identifier_new_move(char* name, int count);
void identifier_free(struct E_Identifier* self);

int identifier_compare(struct E_Identifier* self, struct E_Identifier* other, struct Evaluator* etor);
void identifier_eval(struct E_Identifier* self, struct Evaluator* etor);
void identifier_freeVars(struct E_Identifier* self, struct D_Set* freeVars,struct Evaluator* etor);
char* identifier_getName(struct E_Identifier* self);
HashCode identifier_hashCode(struct E_Identifier* self, struct Evaluator* etor);
struct D_Triple* identifier_match(struct E_Identifier* self, struct Any* other, struct D_Triple* bindings);
void identifier_show(struct E_Identifier* self, FILE* fd);
size_t identifier_sizeOf(struct E_Identifier* self);
size_t identifier_structSize(enum TypeId typeId);

#endif
