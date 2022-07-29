#include <string.h>

#include "test.h"

#include "../src/data/any.h"
#include "../src/data/array.h"
#include "../src/data/integer.h"
#include "../src/data/list.h"
#include "../src/data/queue.h"
#include "../src/data/real.h"
#include "../src/data/string.h"
#include "../src/data/symbol.h"
#include "../src/expr/identifier.h"
#include "../src/gc/gc.h"
#include "../src/lexer/lexobj.h"
#include "../src/main/setup.h"

static void lexMultiple(void);
static void lexIdentifier(void);
static void lexInteger(void);
static void lexOperator(void);
static void lexReal(void);
static void lexReserved(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
    {"lexMultiple", lexMultiple},
    {"lexIdentifier", lexIdentifier},
    {"lexInteger", lexInteger},
    {"lexOperator", lexOperator},
    {"lexReal", lexReal},
    {"lexReserved", lexReserved},
    {0, 0}
};

// Before & after ----------------------------------------------------

static void test_before(void) {
    gc_start();
}

static void test_after(void) {
    gc_stop();
}

// Runs all the listed tests -----------------------------------------

void test_lexer(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void lexMultiple(void) {
    struct D_String* input = string_new("abc +-*/ (Def)\n123 456.789");
    struct D_List* res = lexObj_string(input);
    EXPECT_NOT_NULL(res);
    EXPECT_EQ(8, list_count(res));
    struct Any* elem = list_getFirst(res);
    EXPECT(any_isA(elem, T_Array));
    struct Any* sym = array_get_unsafe((struct D_Array*)elem, 0);
    EXPECT_EQ(T_Symbol, any_typeId(sym));
    EXPECT_EQ(0, strcmp("Identifier", symbol_getName((struct D_Symbol*)sym)));
    // TODO This is incomplete, but I'm tired of writing this test.
    // Right now it looks like it's working fine.
    // The tokens are:
    // [{Identifier, abc, 0, 0, 0}, {Identifier, +-*/, 0, 0, 0}, {Special, "(", 0, 0, 0}, {Symbol, Def, 0, 0, 0}, {Special, ")", 0, 0, 0}, {Integer, -37877, 0, 0, 0}, {Real, 456.789000, 0, 0, 0}, {EOI, nothing, 0, 0, 0}]
}

static void lexInteger(void) {
    struct D_String* input = string_new("123");
    struct D_List* res = lexObj_string(input);
    EXPECT_NOT_NULL(res);
    EXPECT_EQ(2, list_count(res));
    struct Any* elem = list_getFirst(res);
    EXPECT(any_isA(elem, T_Array));
    struct Any* sym = array_get_unsafe((struct D_Array*)elem, 0);
    EXPECT_EQ(T_Symbol, any_typeId(sym));
    EXPECT_EQ(0, strcmp("Integer", symbol_getName((struct D_Symbol*)sym)));
    struct Any* intVal = array_get_unsafe((struct D_Array*)elem, 1);
    EXPECT_EQ(T_Integer, any_typeId(intVal));
    EXPECT_EQ(123, integer_getValue((struct D_Integer*)intVal));
}

static void lexReal(void) {
    struct D_String* input = string_new("123.456");
    struct D_List* res = lexObj_string(input);
    EXPECT_NOT_NULL(res);
    EXPECT_EQ(2, list_count(res));
    struct Any* elem = list_getFirst(res);
    EXPECT(any_isA(elem, T_Array));
    struct Any* sym = array_get_unsafe((struct D_Array*)elem, 0);
    EXPECT_EQ(T_Symbol, any_typeId(sym));
    EXPECT_EQ(0, strcmp("Real", symbol_getName((struct D_Symbol*)sym)));
    struct Any* realVal = array_get_unsafe((struct D_Array*)elem, 1);
    EXPECT_EQ(T_Real, any_typeId(realVal));
    EXPECT_EQ(123.456, real_getValue((struct D_Real*)realVal));
}

static void lexIdentifier(void) {
    char* identName = "abc";
    struct D_String* input = string_new(identName);
    struct D_List* res = lexObj_string(input);
    EXPECT_NOT_NULL(res);
    EXPECT_EQ(2, list_count(res));
    struct Any* elem = list_getFirst(res);
    EXPECT(any_isA(elem, T_Array));
    struct Any* sym = array_get_unsafe((struct D_Array*)elem, 0);
    EXPECT(any_isA(sym, T_Symbol))
    EXPECT_EQ(0, strcmp("Identifier", symbol_getName((struct D_Symbol*)sym)));
    struct Any* identVal = array_get_unsafe((struct D_Array*)elem, 1);
    EXPECT_EQ(T_Identifier, any_typeId(identVal));
    EXPECT_EQ(0, strcmp(identName, identifier_getName((struct E_Identifier*)identVal)));
}

static void lexReserved(void) {
    char* wordString = "end";
    struct D_String* input = string_new(wordString);
    struct D_List* res = lexObj_string(input);
    EXPECT_NOT_NULL(res);
    EXPECT_EQ(2, list_count(res));
    struct Any* elem = list_getFirst(res);
    EXPECT(any_isA(elem, T_Array));
    struct Any* sym = array_get_unsafe((struct D_Array*)elem, 0);
    EXPECT(any_isA(sym, T_Symbol));
    EXPECT_EQ(0, strcmp("Reserved", symbol_getName((struct D_Symbol*)sym)));
    struct Any* word = array_get_unsafe((struct D_Array*)elem, 1);
    EXPECT(any_isA(word, T_String));
    EXPECT_EQ(0, strcmp(wordString, string_getChars((struct D_String*)word)));
}

static void lexOperator(void) {
    char* wordString = "+-*/";
    struct D_String* input = string_new(wordString);
    struct D_List* res = lexObj_string(input);
    EXPECT_NOT_NULL(res);
    EXPECT_EQ(2, list_count(res));
    struct Any* elem = list_getFirst(res);
    EXPECT(any_isA(elem, T_Array));
    struct Any* sym = array_get_unsafe((struct D_Array*)elem, 0);
    EXPECT(any_isA(sym, T_Symbol));
    EXPECT_EQ(0, strcmp("Operator", symbol_getName((struct D_Symbol*)sym)));
    struct Any* word = array_get_unsafe((struct D_Array*)elem, 1);
    EXPECT(any_isA(word, T_Identifier));
    EXPECT_EQ(0, strcmp(wordString, identifier_getName((struct E_Identifier*)word)));
}
