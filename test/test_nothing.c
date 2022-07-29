#include "test.h"

#include "../src/data/nothing.h"

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

void test_nothing(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void newAndDelete(void) {
    struct D_Nothing* nothing = nothing_new();
    ASSERT_NOT_NULL(nothing);
    nothing_free(nothing);
}
