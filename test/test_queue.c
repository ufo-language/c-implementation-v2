#include "test.h"

#include "../src/data/any.h"
#include "../src/data/integer.h"
#include "../src/data/queue.h"
#include "../src/gc/gc.h"
#include "../src/main/setup.h"

static void emptyCount(void);
static void enqDeq(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
    {"emptyCount", emptyCount},
    {"enqDeq", enqDeq},
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

void test_queue(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void emptyCount(void) {
    struct D_Queue* q = queue_new();
    EXPECT_EQ(0, queue_count(q));
}

static void enqDeq(void) {
    struct D_Queue* q = queue_new();
    struct Any* i100 = (struct Any*)integer_new(100);
    struct Any* i200 = (struct Any*)integer_new(200);
    struct Any* i300 = (struct Any*)integer_new(300);
    EXPECT_EQ(0, queue_count(q));
    queue_enq(q, i100);
    EXPECT_EQ(1, queue_count(q));
    queue_enq(q, i200);
    EXPECT_EQ(2, queue_count(q));
    queue_enq(q, i300);
    EXPECT_EQ(3, queue_count(q));
    struct Any* obj1 = queue_deq_unsafe(q);
    EXPECT_EQ(2, queue_count(q));
    struct Any* obj2 = queue_deq_unsafe(q);
    EXPECT_EQ(1, queue_count(q));
    struct Any* obj3 = queue_deq_unsafe(q);
    EXPECT_EQ(0, queue_count(q));
    EXPECT_EQ(obj1, i100);
    EXPECT_EQ(obj2, i200);
    EXPECT_EQ(obj3, i300);
}
