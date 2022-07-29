#include "test.h"

#include "../src/data/any.h"
#include "../src/data/integer.h"

static void newAndDelete(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
    {"newAndDelete", newAndDelete},
    {0, 0}
};

// Before & after ----------------------------------------------------

static void test_before(void) {}

static void test_after(void) {}

// Runs all the listed tests -----------------------------------------

void test_integer(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void newAndDelete(void) {
    struct D_Integer* integer = integer_new(123); 
    ASSERT(any_isA((struct Any*)integer, T_Integer))
    ASSERT_NOT_NULL(integer);
    EXPECT_EQ(123, integer_getValue(integer));
    integer_free(integer);
}
