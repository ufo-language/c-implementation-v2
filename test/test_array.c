#include "test.h"

#include "../src/data/any.h"
#include "../src/data/array.h"
#include "../src/data/integer.h"
#include "../src/main/globals.h"

static void count(void);
static void getAndSet(void);
static void newAndDelete(void);
static void newAndDelete2(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
    {"newAndDelete", newAndDelete},
    {"newAndDelete2", newAndDelete2},
    {"getAndSet", getAndSet},
    {"count", count},
    {0, 0}
};

// Before & after ----------------------------------------------------

static void test_before() {}

static void test_after() {}

// Runs all the listed tests -----------------------------------------

void test_array() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void newAndDelete(void) {
    int arraySize = 4;
    struct D_Array* ary = array_new(arraySize);
    ASSERT(any_isA((struct Any*)ary, T_Array));
    ASSERT_NOT_NULL(ary);
    for (int n=0; n<arraySize; n++) {
        EXPECT_EQ(NOTHING, array_get_unsafe(ary, n));
    }
    array_free(ary);
}

static void newAndDelete2(void) {
    struct D_Integer* i100 = integer_new(100);
    struct D_Integer* i200 = integer_new(200);
    struct D_Integer* i300 = integer_new(300);
    struct D_Integer* i400 = integer_new(400);
    int arraySize = 4;
    struct D_Array* ary = array_newN(arraySize, i100, i200, i300, i400);
    ASSERT(any_isA((struct Any*)ary, T_Array));
    ASSERT_NOT_NULL(ary);
    EXPECT_EQ(i100, array_get_unsafe(ary, 0));
    EXPECT_EQ(i200, array_get_unsafe(ary, 1));
    EXPECT_EQ(i300, array_get_unsafe(ary, 2));
    EXPECT_EQ(i400, array_get_unsafe(ary, 3));
    array_free(ary);
    integer_free(i100);
    integer_free(i200);
    integer_free(i300);
    integer_free(i400);
}

static void getAndSet(void) {
    struct D_Array* ary = array_new(2);
    struct D_Integer* i100 = integer_new(100);
    EXPECT_EQ(NOTHING, array_get_unsafe(ary, 0));
    EXPECT_EQ(NOTHING, array_get_unsafe(ary, 1));
    array_set_unsafe(ary, 0, (struct Any*)i100);
    EXPECT_EQ(i100, array_get_unsafe(ary, 0));
    EXPECT_EQ(NOTHING, array_get_unsafe(ary, 1));
    integer_free(i100);
    array_free(ary);
}

static void count(void) {
    struct D_Array* ary1 = array_new(4);
    EXPECT_EQ(4, array_count(ary1));
    struct D_Array* ary2 = array_new(7);
    EXPECT_EQ(7, array_count(ary2));
    array_free(ary1);
    array_free(ary2);
}
