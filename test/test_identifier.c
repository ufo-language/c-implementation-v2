#include <string.h>

#include "test.h"

#include "../src/data/any.h"
#include "../src/expr/identifier.h"
#include "../src/gc/gc.h"
#include "../src/main/setup.h"

static void allocDealloc(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
    {"allocDealloc", allocDealloc},
    {0, 0}
};

// Before & after ----------------------------------------------------

static void test_before(void) {
    gc_startFunctions(setup_permanentObjects, setup_rootObjects, NULL);
}

static void test_after(void) {
    gc_stop();
}

// Runs all the listed tests -----------------------------------------

void test_identifier(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void allocDealloc(void) {
    struct E_Identifier* i1 = identifier_new("abc");
    struct E_Identifier* i2 = identifier_new("abc");
    struct E_Identifier* i3 = identifier_new("def");
    EXPECT_EQ(i1, i2);
    EXPECT_NE(i1, i3);
}
