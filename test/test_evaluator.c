#include "test.h"

#include "../src/data/any.h"
#include "../src/data/integer.h"
#include "../src/etor/evaluator.h"
#include "../src/expr/identifier.h"
#include "../src/expr/if.h"
#include "../src/gc/gc.h"
#include "../src/main/globals.h"
#include "../src/main/setup.h"

static void allocDealloc(void);
static void pushPopObj(void);
static void bindLookup(void);
static void pushPopExpr1(void);
static void pushPopExpr2(void);
static void testIf(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
    {"allocDealloc", allocDealloc},
    {"pushPopObj", pushPopObj},
    {"bindLookup", bindLookup},
    {"pushPopExpr1", pushPopExpr1},
    {"pushPopExpr2", pushPopExpr2},
    {"testIf", testIf},
    {0, 0}
};

// Before & after ----------------------------------------------------

static struct Evaluator* etor;

void _rootObjects(void) {
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

void test_evaluator(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void allocDealloc(void) {
    EXPECT_NOT_NULL(etor);
}

static void pushPopObj(void) {
    struct D_Integer* i100 = integer_new(100);
    struct D_Integer* i200 = integer_new(100);
    evaluator_pushObj(etor, (struct Any*)i100);
    evaluator_pushObj(etor, (struct Any*)i200);
    struct Any* obj1 = evaluator_popObj(etor);
    struct Any* obj2 = evaluator_popObj(etor);
    EXPECT_EQ(i200, obj1);
    EXPECT_EQ(i100, obj2);
}

static void bindLookup(void) {
    struct E_Identifier* x = identifier_new("x");
    struct D_Integer* i100 = integer_new(100);
    evaluator_bind(etor, x, (struct Any*)i100);
    struct Any* obj = evaluator_lookup(etor, x);
    EXPECT_EQ(i100, obj);
    struct E_Identifier* y = identifier_new("y");
    struct D_Integer* i200 = integer_new(200);
    evaluator_bind(etor, y, (struct Any*)i200);
    obj = evaluator_lookup(etor, y);
    EXPECT_EQ(i200, obj);
    obj = evaluator_lookup(etor, x);
    EXPECT_EQ(i100, obj);
}

static void pushPopExpr1(void) {
    struct D_Integer* i100 = integer_new(100);
    evaluator_pushExpr(etor, (struct Any*)i100);
    struct Any* obj = evaluator_popExpr(etor);
    EXPECT_EQ(i100, obj);
}

static void pushPopExpr2(void) {
    struct E_Identifier* x = identifier_new("x");
    struct E_Identifier* y = identifier_new("y");
    struct D_Integer* i100 = integer_new(100);
    struct D_Integer* i200 = integer_new(200);
    struct D_Integer* i300 = integer_new(300);
    struct D_Integer* i400 = integer_new(400);
    evaluator_bind(etor, x, (struct Any*)i100);
    evaluator_pushExpr(etor, (struct Any*)i200);
    evaluator_bind(etor, y, (struct Any*)i300);
    evaluator_pushExpr(etor, (struct Any*)i400);
    struct Any* obj;
    obj = evaluator_lookup(etor, x);
    EXPECT_EQ(i100, obj);
    obj = evaluator_lookup(etor, y);
    EXPECT_EQ(i300, obj);
    obj = evaluator_popExpr(etor);
    EXPECT_EQ(i400, obj);
    obj = evaluator_lookup(etor, y);
    EXPECT_EQ(i300, obj);
    obj = evaluator_lookup(etor, x);
    EXPECT_EQ(i100, obj);
    obj = evaluator_popExpr(etor);
    EXPECT_EQ(i200, obj);
    obj = evaluator_lookup(etor, y);
    EXPECT_EQ(NULL, obj);
    obj = evaluator_lookup(etor, x);
    EXPECT_EQ(i100, obj);

}

static void testIf(void) {
    struct D_Integer* i100 = integer_new(100);
    struct D_Integer* i200 = integer_new(200);
    struct E_If* ifExpr = if_new((struct Any*)TRUE, (struct Any*)i100, (struct Any*)i200);
    evaluator_pushExpr(etor, (struct Any*)ifExpr);
    evaluator_run(etor);
    struct Any* res = evaluator_popObj(etor);
    EXPECT_EQ(i100, res);
    ifExpr = if_new((struct Any*)FALSE, (struct Any*)i100, (struct Any*)i200);
    evaluator_pushExpr(etor, (struct Any*)ifExpr);
    evaluator_run(etor);
    res = evaluator_popObj(etor);
    EXPECT_EQ(i200, res);
}
