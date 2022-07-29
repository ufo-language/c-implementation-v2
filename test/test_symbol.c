#include <string.h>

#include "test.h"

#include "../src/data/any.h"
#include "../src/data/symbol.h"
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

void test_symbol(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void allocDealloc(void) {
    struct D_Symbol* s1 = symbol_new("Abc");
    struct D_Symbol* s2 = symbol_new("Abc");
    struct D_Symbol* s3 = symbol_new("Def");
    EXPECT_EQ(s1, s2);
    EXPECT_NE(s1, s3);
}
