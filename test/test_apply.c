#include "test.h"

#include "../src/data/any.h"
#include "../src/data/integer.h"
#include "../src/data/list.h"
#include "../src/etor/evaluator.h"
#include "../src/expr/abstraction.h"
#include "../src/expr/apply.h"
#include "../src/expr/identifier.h"
#include "../src/gc/gc.h"
#include "../src/main/globals.h"
#include "../src/main/setup.h"

static void applyOneArg(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
    {"applyOneArg", applyOneArg},
    {0, 0}
};

// Before & after ----------------------------------------------------

static struct Evaluator* etor;

static void _rootObjects(void) {
    setup_rootObjects();
    etor = evaluator_new();
}

static void test_before(void) {
    gc_startFunctions(setup_permanentObjects, _rootObjects, NULL);
}

static void test_after(void) {
    gc_stop();
}

// Runs all the listed tests -----------------------------------------

void test_apply(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void applyOneArg(void) {
    struct E_Identifier* x = identifier_new("x");
    struct D_List* params = list_new((struct Any*)x, (struct Any*)EMPTY_LIST);
    struct E_Abstraction* abstr = abstraction_new(params, (struct Any*)x);
    struct D_Integer* i100 = integer_new(100);
    struct D_List* args = list_new((struct Any*)i100, (struct Any*)EMPTY_LIST);
    struct E_Apply* apply = apply_new((struct Any*)abstr, args);
    any_eval((struct Any*)apply, etor);
    evaluator_run(etor);
    struct Any* res = evaluator_popObj(etor);
    EXPECT_EQ(i100, res);
}
