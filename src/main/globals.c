#include "data/array.h"
#include "data/boolean.h"
#include "data/file.h"
#include "data/hashtable.h"
#include "data/nil.h"
#include "data/list.h"
#include "data/stream.h"
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
struct D_HashTable* GENERATED_SYMBOL_NUMBERS;
struct D_HashTable* INTERNED_SYMBOLS;
struct D_HashTable* INTERNED_IDENTIFIERS;
struct D_Nil*       NIL;
struct D_Boolean*   TRUE;
struct D_Boolean*   FALSE;

struct D_File*      STDOUT_FILE;
struct D_Stream*    STDOUT_STREAM;

struct D_Symbol*    SYM_ANY;
struct D_Symbol*    SYM_ARRAY;
struct D_Symbol*    SYM_BOOLEAN;
struct D_Symbol*    SYM_IDENTIFIER;
struct D_Symbol*    SYM_INFILE;
struct D_Symbol*    SYM_INTEGER;
struct D_Symbol*    SYM_NIL;
struct D_Symbol*    SYM_OUTFILE;
struct D_Symbol*    SYM_STRING;
struct D_Symbol*    SYM_SYMBOL;

// These are the permanent objects that do not need mark & sweep.
void globals_permanentObjects(void) {
    NIL = nil_new();
    TRUE = boolean_new(true);
    FALSE = boolean_new(false);
    EMPTY_ARRAY = array_new(0);
    EMPTY_LIST = list_new(NULL, NULL);
    EMPTY_STRING = string_new("");
    EMPTY_TRIPLE = triple_new(NULL, NULL, NULL);
    STDOUT_FILE = file_fromFilePointer(string_new("stdout"), stdout);
    STDOUT_STREAM = stream_new((struct Any*)STDOUT_FILE);
}

// These are the root objects that allow their contents to be marked.
void globals_rootObjects(void) {
    GENERATED_SYMBOL_NUMBERS = hashTable_new();
    INTERNED_SYMBOLS = hashTable_new();
    INTERNED_IDENTIFIERS = hashTable_new();
    // These should actually be permanent objects but they can't be created
    // until after the intern hash table is created.
    SYM_ANY = symbol_new("Any");
    SYM_ARRAY = symbol_new("Array");
    SYM_BOOLEAN = symbol_new("Boolean");
    SYM_INFILE = symbol_new("InFile");
    SYM_IDENTIFIER = symbol_new("Identifier");
    SYM_INTEGER = symbol_new("Integer");
    SYM_NIL = symbol_new("Nil");
    SYM_OUTFILE = symbol_new("OutFile");
    SYM_STRING = symbol_new("String");
    SYM_SYMBOL = symbol_new("Symbol");
}
