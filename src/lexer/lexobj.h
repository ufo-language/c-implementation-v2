#if !defined(LEXOBJ_H)
#define LEXOBJ_H

#include "lexer.h"

struct Any;
struct D_List;
struct D_String;

extern struct D_Symbol* LEXER_SYMBOLS[LT_FINAL];

// This is the interface between the lexer (which uses only primitive
// types) and UFO.
struct D_List* lexObj_string(struct D_String* string);

#endif
