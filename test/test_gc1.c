#include "test.h"

#include "../src/data/any.h"
#include "../src/data/integer.h"
#include "../src/gc/gc.h"
#include "../src/main/globals.h"

static void startStop(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
    {"startStop", startStop},
    {0, 0}
};

// Before & after ----------------------------------------------------

static void test_before(void) {}

static void test_after(void) {}

// Runs all the listed tests -----------------------------------------

void test_gc1(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void startStop(void) {
    gc_startFunctions(NULL, NULL, NULL);
    struct D_Integer* i100 = integer_new(100);
    EXPECT(gc_isSpine((struct Any*)i100));
    gc_stop();
    EXPECT_F(gc_isSpine((struct Any*)i100));
}
