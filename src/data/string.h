#if !defined(STRING_H)
#define STRING_H

#include <stdbool.h>
#include <stdio.h>

#include "main/typedefs.h"

struct D_String;
struct Evaluator;

struct D_String* string_fromChar(char c);
struct D_String* string_new(char* name);  // copy semantics
struct D_String* string_new_move(char* str, int count);  // move semantics
void string_free(struct D_String* self);

void string_escapifyChar(char c, FILE* fd);

bool string_boolValue(struct D_String* self);
int string_compare(struct D_String* self, struct D_String* other, struct Evaluator* etor);
int string_count(struct D_String* self);
void string_display(struct D_String* self, FILE* fd);
void string_escapify(struct D_String* self, FILE* fd);
char string_getChar(struct D_String* self, int n, struct Evaluator* etor);
char string_getChar_unsafe(struct D_String* self, int n);
char* string_getChars(struct D_String* self);
HashCode string_hashCode(struct D_String* self, struct Evaluator* etor);
bool string_isEqual(struct D_String* self, struct D_String* other);
struct D_String* string_join(struct D_String* self, struct D_String* other);
void string_show(struct D_String* self, FILE* fd);
struct D_List* string_split(struct D_String* self, char c);
void string_unescapify(struct D_String* self, FILE* fd);
size_t string_sizeOf(struct D_String* self);
size_t string_structSize();

#endif
