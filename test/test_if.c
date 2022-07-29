#include "test.h"

#include "../src/data/any.h"
#include "../src/data/integer.h"
#include "../src/etor/evaluator.h"
#include "../src/expr/if.h"
#include "../src/gc/gc.h"
#include "../src/main/globals.h"
#include "../src/main/setup.h"

static void trueCondition(void);
static void falseCondition(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
    {"trueCondition", trueCondition},
    {"falseCondition", falseCondition},
    {0, 0}
};

// Before & after ----------------------------------------------------

static struct Evaluator* etor;

static void _rootObjects(void) {
    etor = evaluator_new();
}

static void test_before(void) {
    gc_startFunctions(setup_permanentObjects, _rootObjects, NULL);
}

static void test_after(void) {
    gc_stop();
}

// Runs all the listed tests -----------------------------------------

void test_if(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void trueCondition(void) {
    struct Any* i100 = (struct Any*)integer_new(100);
    struct Any* i200 = (struct Any*)integer_new(200);
    struct Any* ifExpr = (struct Any*)if_new((struct Any*)TRUE, i100, i200);
    any_eval((struct Any*)ifExpr, etor);
    evaluator_run(etor);
    struct Any* res = evaluator_popObj(etor);
    EXPECT_EQ(i100, res);
}

static void falseCondition(void) {
    struct Any* i100 = (struct Any*)integer_new(100);
    struct Any* i200 = (struct Any*)integer_new(200);
    struct Any* ifExpr = (struct Any*)if_new((struct Any*)FALSE, i100, i200);
    any_eval((struct Any*)ifExpr, etor);
    evaluator_run(etor);
    struct Any* res = evaluator_popObj(etor);
    EXPECT_EQ(i200, res);
}
