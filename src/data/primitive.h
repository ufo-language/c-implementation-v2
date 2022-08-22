#if !defined(PRIMITIVE_H)
#define PRIMITIVE_H

#include <stdbool.h>
#include <stdio.h>

#include "data/any.h"
#include "main/typedefs.h"

struct D_HashTable;
struct D_List;
struct D_Primitive;
struct Evaluator;

typedef void (*PrimitiveFunction)(struct Evaluator* etor, struct D_List* args);

void primitive_argCountException(int nArgs, struct D_List* argList, struct Evaluator* etor);
void primitive_argTypeException(enum TypeId expectedTypeId, enum TypeId foundTypeId, struct Any* argument, struct Evaluator* etor);
void primitive_checkArgs(int nParams, enum TypeId paramTypes[], struct D_List* argList, struct Any** paramVars[], struct Evaluator* etor);
int primitive_checkArgs2(int nParams1, int nParams2, enum TypeId paramTypes[], struct D_List* argList, struct Any** paramVars[], struct Evaluator* etor);
void primitive_define(struct D_HashTable* namespace, char* name, PrimitiveFunction prim);
void primitive_defineMacro(struct D_HashTable* namespace, char* name, PrimitiveFunction prim);

struct D_Primitive* primitive_new(PrimitiveFunction primFunc, char* name, bool isMacro);
void primitive_free(struct D_Primitive* self);

void primitive_apply(struct D_Primitive* self, struct D_List* args, struct Evaluator* etor);
bool primitive_isMacro(struct D_Primitive* self);
HashCode primitive_hashCode(struct D_Primitive* self, struct Evaluator* etor);
void primitive_show(struct D_Primitive* self, FILE* fp);
size_t primitive_sizeOf(struct D_Primitive* self);
size_t primitive_structSize(void);

#endif
