#include "test.h"

#include "../src/data/any.h"
#include "../src/data/integer.h"
#include "../src/data/hashtable.h"
#include "../src/etor/evaluator.h"
#include "../src/expr/identifier.h"
#include "../src/gc/gc.h"
#include "../src/main/setup.h"

static void putGet(void);
static void resize(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
    {"putGet", putGet},
    {"resize", resize},
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

void test_hashTable(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void putGet(void) {
    struct D_HashTable* hash = hashTable_new();
    struct E_Identifier* x = identifier_new("x");
    struct E_Identifier* y = identifier_new("y");
    struct D_Integer* i100 = integer_new(100);
    struct D_Integer* i200 = integer_new(200);
    hashTable_put(hash, (struct Any*)x, (struct Any*)i100, etor);
    struct Any* val = hashTable_get(hash, (struct Any*)x, etor);
    EXPECT_EQ(i100, val);
    any_show((struct Any*)hash, stdout);
    any_show((struct Any*)y, stdout);
    any_show((struct Any*)etor, stdout);

    // This causes a segfault because no jump_buf has been set up
    val = hashTable_get(hash, (struct Any*)y, etor);
    EXPECT_NULL(val);
    hashTable_put(hash, (struct Any*)y, (struct Any*)i200, etor);
    val = hashTable_get(hash, (struct Any*)x, etor);
    EXPECT_EQ(i100, val);
    val = hashTable_get(hash, (struct Any*)y, etor);
    EXPECT_EQ(i200, val);
}

static void resize(void) {
    // an 8-bucket hash table should resize on the 7th entry
    struct D_HashTable* hash = hashTable_new();
    struct E_Identifier* a = identifier_new("a");
    struct E_Identifier* b = identifier_new("b");
    struct E_Identifier* c = identifier_new("c");
    struct E_Identifier* d = identifier_new("d");
    struct E_Identifier* e = identifier_new("e");
    struct E_Identifier* f = identifier_new("f");
    struct E_Identifier* g = identifier_new("g");
    struct D_Integer* i100 = integer_new(100);
    struct D_Integer* i200 = integer_new(200);
    struct D_Integer* i300 = integer_new(300);
    struct D_Integer* i400 = integer_new(400);
    struct D_Integer* i500 = integer_new(500);
    struct D_Integer* i600 = integer_new(600);
    struct D_Integer* i700 = integer_new(700);
    hashTable_put(hash, (struct Any*)a, (struct Any*)i100, etor);
    hashTable_put(hash, (struct Any*)b, (struct Any*)i200, etor);
    hashTable_put(hash, (struct Any*)c, (struct Any*)i300, etor);
    hashTable_put(hash, (struct Any*)d, (struct Any*)i400, etor);
    hashTable_put(hash, (struct Any*)e, (struct Any*)i500, etor);
    hashTable_put(hash, (struct Any*)f, (struct Any*)i600, etor);
    EXPECT_EQ(8, hashTable_nBuckets(hash));
    hashTable_put(hash, (struct Any*)g, (struct Any*)i700, etor);
    EXPECT_EQ(i100, hashTable_get(hash, (struct Any*)a, etor));
    EXPECT_EQ(i200, hashTable_get(hash, (struct Any*)b, etor));
    EXPECT_EQ(i300, hashTable_get(hash, (struct Any*)c, etor));
    EXPECT_EQ(i400, hashTable_get(hash, (struct Any*)d, etor));
    EXPECT_EQ(i500, hashTable_get(hash, (struct Any*)e, etor));
    EXPECT_EQ(i600, hashTable_get(hash, (struct Any*)f, etor));
    EXPECT_EQ(i700, hashTable_get(hash, (struct Any*)g, etor));
    EXPECT_EQ(12, hashTable_nBuckets(hash));
}
