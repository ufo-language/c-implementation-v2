#include "data/array.h"
#include "data/boolean.h"
#include "data/hashtable.h"
#include "data/nil.h"
#include "data/list.h"
#include "data/string.h"
#include "data/symbol.h"
#include "data/triple.h"
#include "main/globals.h"

struct D_Array;
struct D_HashTable;
struct D_Nil;
struct D_List;
struct D_String;
struct D_Triple;

struct D_Array*     EMPTY_ARRAY;
struct D_List*      EMPTY_LIST;
struct D_String*    EMPTY_STRING;
struct D_Triple*    EMPTY_TRIPLE;
struct D_HashTable* INTERNED_SYMBOLS;
struct D_HashTable* INTERNED_IDENTIFIERS;
struct D_Nil*       NIL;
struct D_Boolean*   TRUE;
struct D_Boolean*   FALSE;

struct D_Symbol*    SYM_ANY;
struct D_Symbol*    SYM_ARRAY;
struct D_Symbol*    SYM_BOOLEAN;
struct D_Symbol*    SYM_IDENTIFIER;
struct D_Symbol*    SYM_INTEGER;
struct D_Symbol*    SYM_NIL;
struct D_Symbol*    SYM_STRING;
struct D_Symbol*    SYM_SYMBOL;

void globals_permanentObjects(void) {
    NIL = nil_new();
    TRUE = boolean_new(true);
    FALSE = boolean_new(false);
    EMPTY_ARRAY = array_new(0);
    EMPTY_LIST = list_new(NULL, NULL);
    EMPTY_STRING = string_new("");
    EMPTY_TRIPLE = triple_new(NULL, NULL, NULL);
}

void globals_rootObjects(void) {
    INTERNED_SYMBOLS = hashTable_new();
    INTERNED_IDENTIFIERS = hashTable_new();
    SYM_ANY = symbol_new("Any");
    SYM_ARRAY = symbol_new("Array");
    SYM_BOOLEAN = symbol_new("Boolean");
    SYM_IDENTIFIER = symbol_new("Identifier");
    SYM_INTEGER = symbol_new("Integer");
    SYM_NIL = symbol_new("Nil");
    SYM_STRING = symbol_new("String");
    SYM_SYMBOL = symbol_new("Symbol");
}
