#if !defined(GLOBALS_H)
#define GLOBALS_H

struct D_Array;
struct D_HashTable;
struct D_Nil;
struct D_List;
struct D_String;
struct D_Triple;

extern struct D_Array*     EMPTY_ARRAY;
extern struct D_List*      EMPTY_LIST;
extern struct D_String*    EMPTY_STRING;
extern struct D_Triple*    EMPTY_TRIPLE;
extern struct D_HashTable* INTERNED_SYMBOLS;
extern struct D_HashTable* INTERNED_IDENTIFIERS;
extern struct D_Nil*       NIL;
extern struct D_Boolean*   TRUE;
extern struct D_Boolean*   FALSE;

extern struct D_File*      STDOUT_FILE;
extern struct D_Stream*    STDOUT_STREAM;

// symbols represnting type names
extern struct D_Symbol*    SYM_ANY;
extern struct D_Symbol*    SYM_ARRAY;
extern struct D_Symbol*    SYM_BOOLEAN;
extern struct D_Symbol*    SYM_IDENTIFIER;
extern struct D_Symbol*    SYM_INFILE;
extern struct D_Symbol*    SYM_INTEGER;
extern struct D_Symbol*    SYM_NIL;
extern struct D_Symbol*    SYM_OUTFILE;
extern struct D_Symbol*    SYM_STRING;
extern struct D_Symbol*    SYM_SYMBOL;

// symbols for thread statuses
extern struct D_Symbol*    SYM_RUNNING;
extern struct D_Symbol*    SYM_BLOCKED;
extern struct D_Symbol*    SYM_TERMINATED;

#endif
