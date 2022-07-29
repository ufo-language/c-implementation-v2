#include "test.h"

#include "../src/data/any.h"
#include "../src/data/integer.h"
#include "../src/gc/gc.h"

static void sizeOfInteger(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
    {"sizeOfInteger", sizeOfInteger},
    {0, 0}
};

// Before & after ----------------------------------------------------

static void test_before(void) {
    gc_startFunctions(NULL, NULL, NULL);
}

static void test_after(void) {
    gc_stop();
}

// Runs all the listed tests -----------------------------------------

void test_sizeOf(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void sizeOfInteger(void) {
    size_t expectedSize = sizeof(struct Any) + sizeof(struct Any*);
    struct D_Integer* i100 = integer_new(100);
    size_t actualSize = any_sizeOf((struct Any*)i100);
    EXPECT_EQ(expectedSize, actualSize);
}
