#include "test.h"

#include "../src/data/any.h"
#include "../src/data/integer.h"
#include "../src/data/list.h"
#include "../src/gc/gc.h"
#include "../src/main/globals.h"
#include "../src/main/setup.h"

static void collect(void);
static void commitAndSweep1(void);
static void commitAndSweep2(void);
static void globals(void);
static void globalsSetup(void);
static void markInteger(void);
static void markList(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
    {"markInteger", markInteger},
    {"markList", markList},
    {"commitAndSweep1", commitAndSweep1},
    {"commitAndSweep2", commitAndSweep2},
    {"globals", globals},
    {"globalsSetup", globalsSetup},
    {"collect", collect},
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

void test_gc2(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void markInteger(void) {
    struct D_Integer* i100 = integer_new(100);
    struct D_Integer* i200 = integer_new(200);
    // check the isMarked flags directly
    EXPECT_F(((struct Any*)i100)->isMarked);
    EXPECT_F(((struct Any*)i200)->isMarked);
    // use the any_isMarked method
    EXPECT_F(any_isMarked((struct Any*)i100));
    EXPECT_F(any_isMarked((struct Any*)i200));
    // mark the objects
    any_mark((struct Any*)i100);
    any_mark((struct Any*)i200);
    // check the flags again
    EXPECT(((struct Any*)i100)->isMarked);
    EXPECT(((struct Any*)i200)->isMarked);
    EXPECT(any_isMarked((struct Any*)i100));
    EXPECT(any_isMarked((struct Any*)i200));
}

static void markList(void) {
    struct D_Integer* i100 = integer_new(100);
    struct D_Integer* i200 = integer_new(200);
    struct D_List* list = list_new((struct Any*)i100, (struct Any*)i200);
    EXPECT_F(((struct Any*)i100)->isMarked);
    EXPECT_F(((struct Any*)i200)->isMarked);
    EXPECT_F(((struct Any*)list)->isMarked);
    any_mark((struct Any*)list);
    EXPECT(((struct Any*)i100)->isMarked);
    EXPECT(((struct Any*)i200)->isMarked);
    EXPECT(((struct Any*)list)->isMarked);
}

void commitAndSweep1(void) {
    struct D_Integer* i100 = integer_new(100);
    EXPECT(gc_isSpine((struct Any*)i100));
    gc_sweep();
    // the object is not committed, so it should not be swept
    EXPECT(gc_isSpine((struct Any*)i100));
    EXPECT_F(gc_isCommitted((struct Any*)i100));
    gc_commit();
    EXPECT(gc_isCommitted((struct Any*)i100));
    gc_sweep();
    // the object is committed, so it should be swept
    EXPECT_F(gc_isSpine((struct Any*)i100));
}

void commitAndSweep2(void) {
    struct D_Integer* i100 = integer_new(100);
    EXPECT(gc_isSpine((struct Any*)i100));
    EXPECT_F(gc_isCommitted((struct Any*)i100));
    gc_commit();
    EXPECT(gc_isSpine((struct Any*)i100));
    EXPECT(gc_isCommitted((struct Any*)i100));
    struct D_Integer* i200 = integer_new(200);
    EXPECT(gc_isSpine((struct Any*)i100));
    EXPECT(gc_isCommitted((struct Any*)i100));
    EXPECT(gc_isSpine((struct Any*)i200));
    EXPECT_F(gc_isCommitted((struct Any*)i200));
    gc_sweep();
    // i100 is committed so it should be swept
    EXPECT_F(gc_isSpine((struct Any*)i100));
    EXPECT_F(gc_isCommitted((struct Any*)i100));
    // i200 is not committed so ut should not be swept
    EXPECT(gc_isSpine((struct Any*)i200));
}

void globals(void) {
    struct D_Integer* i100 = integer_new(100);
    EXPECT(gc_isSpine((struct Any*)i100));
    EXPECT_F(gc_isCommitted((struct Any*)i100));
    gc_commitPermanentObjects();
    EXPECT(gc_isSpine((struct Any*)i100));
    EXPECT(gc_isCommitted((struct Any*)i100));
    EXPECT(gc_isPermanent((struct Any*)i100));
    gc_sweep();
    EXPECT(gc_isSpine((struct Any*)i100));
    EXPECT(gc_isCommitted((struct Any*)i100));
    EXPECT(gc_isPermanent((struct Any*)i100));
    struct D_Integer* i200 = integer_new(200);
    EXPECT(gc_isSpine((struct Any*)i200));
    EXPECT_F(gc_isCommitted((struct Any*)i200));
    gc_commit();
    EXPECT(gc_isSpine((struct Any*)i200));
    EXPECT(gc_isCommitted((struct Any*)i200));
    struct D_Integer* i300 = integer_new(300);
    EXPECT(gc_isSpine((struct Any*)i300));
    EXPECT_F(gc_isCommitted((struct Any*)i300));
    gc_sweep();
    EXPECT(gc_isSpine((struct Any*)i100));
    EXPECT(gc_isCommitted((struct Any*)i100));
    EXPECT(gc_isPermanent((struct Any*)i100));
    EXPECT_F(gc_isSpine((struct Any*)i200));
    EXPECT_F(gc_isCommitted((struct Any*)i200));
    EXPECT_F(gc_isPermanent((struct Any*)i200));
    EXPECT(gc_isSpine((struct Any*)i300));
    EXPECT_F(gc_isCommitted((struct Any*)i300));
    EXPECT_F(gc_isPermanent((struct Any*)i300));
}

void globalsSetup(void) {
    EXPECT(gc_isPermanent((struct Any*)NOTHING));
    EXPECT(gc_isPermanent((struct Any*)TRUE));
    EXPECT(gc_isPermanent((struct Any*)FALSE));
    EXPECT(gc_isPermanent((struct Any*)EMPTY_LIST));
    EXPECT(gc_isPermanent((struct Any*)EMPTY_TRIPLE));
    gc_sweep();
    EXPECT(gc_isPermanent((struct Any*)NOTHING));
    EXPECT(gc_isPermanent((struct Any*)TRUE));
    EXPECT(gc_isPermanent((struct Any*)FALSE));
    EXPECT(gc_isPermanent((struct Any*)EMPTY_LIST));
    EXPECT(gc_isPermanent((struct Any*)EMPTY_TRIPLE));
}

void collect(void) {
    struct D_Integer* i100 = integer_new(100);
    struct D_Integer* i200 = integer_new(200);
    EXPECT(gc_isSpine((struct Any*)i100));
    EXPECT(gc_isSpine((struct Any*)i200));
    gc_collect();
    EXPECT(gc_isSpine((struct Any*)i100));
    EXPECT(gc_isSpine((struct Any*)i200));
    gc_commit();
    EXPECT(gc_isCommitted((struct Any*)i100));
    EXPECT(gc_isCommitted((struct Any*)i200));
    gc_collect();
    EXPECT_F(gc_isSpine((struct Any*)i100));
    EXPECT_F(gc_isSpine((struct Any*)i200));
}
