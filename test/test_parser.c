#include "test.h"

#include "../src/data/any.h"
#include "../src/data/array.h"
#include "../src/gc/gc.h"
#include "../src/lexer/lexobj.h"
#include "../src/parser/parser.h"
#include "../src/data/integer.h"
#include "../src/data/list.h"
#include "../src/data/string.h"
#include "../src/main/setup.h"

//static void parseInt(void);
//void parser2_test(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
//    {"parseInt", parseInt},
//    {"parser2_test", parser2_test},
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

void test_parser(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

#if 0
struct ParseState;
struct D_List* _parse_int(struct ParseState* parseState);

static void parseInt(void) {
    struct D_String* inputString = string_new("100");
    struct D_List* tokens = lex_string(inputString);
    struct D_Array* box = array_new(1);
    parse(tokens, box, _parse_int);
    struct Any* parseRes = array_get_unsafe(box, 0);
    EXPECT(any_isA(parseRes, T_List));
    struct D_List* parseResList = (struct D_List*)parseRes;
    struct Any* firstElem = list_getFirst(parseResList);
    EXPECT(any_isA(firstElem, T_Integer));
    struct D_Integer* intElem = (struct D_Integer*)firstElem;
    EXPECT_EQ(100, integer_getValue(intElem));
}
#endif
