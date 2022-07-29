#include "test.h"

#include "../src/data/any.h"
#include "../src/data/boolean.h"
#include "../src/gc/gc.h"
#include "../src/main/globals.h"
#include "../src/main/setup.h"

static void boolValue(void);
static void newAndUnique(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
    {"newAndUnique", newAndUnique},
    {"boolValue", boolValue},
    {0, 0}
};

// Before & after ----------------------------------------------------

static void test_before(void) {
    gc_startFunctions(setup_permanentObjects, NULL, NULL);
}

static void test_after(void) {
    gc_stop();
}

// Runs all the listed tests -----------------------------------------

void test_boolean(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void newAndUnique(void) {
    ASSERT_NOT_NULL(TRUE);
    ASSERT_NOT_NULL(FALSE);
    EXPECT_NE(TRUE, FALSE);
}

static void boolValue(void) {
    EXPECT(any_boolValue((struct Any*)TRUE));
    EXPECT_F(any_boolValue((struct Any*)FALSE));
}
