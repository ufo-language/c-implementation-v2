#include "test.h"

#include "../src/data/any.h"
#include "../src/data/array.h"
#include "../src/data/integer.h"
#include "../src/etor/evaluator.h"
#include "../src/expr/do.h"
#include "../src/gc/gc.h"
#include "../src/main/globals.h"
#include "../src/main/setup.h"

static void do0(void);
static void do1(void);
static void do2(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
    {"do0", do0},
    {"do1", do1},
    {"do2", do2},
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

void test_do(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void do0(void) {
    struct D_Array* ary0 = array_new(0);
    struct Any* doExpr = (struct Any*)do_new(ary0);
    //any_eval(doExpr, etor);
    evaluator_pushExpr(etor, doExpr);
    evaluator_run(etor);
    struct Any* res = evaluator_popObj(etor);
    EXPECT_EQ(NOTHING, res);
}

static void do1(void) {
    struct D_Array* ary1 = array_new(1);
    struct Any* i100 = (struct Any*)integer_new(100);
    array_set_unsafe(ary1, 0, i100);
    struct Any* doExpr = (struct Any*)do_new(ary1);
    //any_eval(doExpr, etor);
    evaluator_pushExpr(etor, doExpr);
    evaluator_setShowSteps(etor, true);
    evaluator_run(etor);
    struct Any* res = evaluator_popObj(etor);
    EXPECT_EQ(i100, res);
}

static void do2(void) {
    struct D_Array* ary2 = array_new(2);
    struct Any* i100 = (struct Any*)integer_new(100);
    struct Any* i200 = (struct Any*)integer_new(200);
    array_set_unsafe(ary2, 0, i100);
    array_set_unsafe(ary2, 1, i200);
    struct Any* doExpr = (struct Any*)do_new(ary2);
    //any_eval(doExpr, etor);
    evaluator_pushExpr(etor, doExpr);
    evaluator_run(etor);
    struct Any* res = evaluator_popObj(etor);
    EXPECT_EQ(i200, res);
}
