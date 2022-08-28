#if !defined(SYMBOL_H)
#define SYMBOL_H

#include <stdbool.h>
#include <stdio.h>

#include "main/typedefs.h"  // for HashCode

struct D_String;
struct D_Symbol;
struct Evaluator;

struct D_Symbol* symbol_new(char* name);
struct D_Symbol* symbol_new_move(char* name, int count);
void symbol_free(struct D_Symbol* self);

int symbol_compare(struct D_Symbol* self, struct D_Symbol* other, struct Evaluator* etor);
char* symbol_getName(struct D_Symbol* self);
bool symbol_hashCode(struct D_Symbol* self, HashCode* hashCode);
struct D_Symbol* symbol_lookup(struct D_String* name);
void symbol_show(struct D_Symbol* self, FILE* fd);
size_t symbol_sizeOf(struct D_Symbol* self);
size_t symbol_structSize(void);

#endif
