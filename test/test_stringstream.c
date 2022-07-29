#include "test.h"

#include "../src/data/any.h"
#include "../src/data/stringstream.h"
#include "../src/gc/gc.h"
#include "../src/main/setup.h"

static void allocDealloc(void);
static void get(void);
static void pos(void);
static void unget(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
    {"allocDealloc", allocDealloc},
    {"get", get},
    {"pos", pos},
    {"unget", unget},
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

void test_stringStream(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void allocDealloc(void) {
    struct D_StringStream* ss = stringStream_new("abc");
    EXPECT_NOT_NULL(ss);
}

static void get(void) {
    struct D_StringStream* ss = stringStream_new("abc");
    EXPECT_EQ('a', stringStream_get(ss));
    EXPECT_EQ('b', stringStream_get(ss));
    EXPECT_EQ('c', stringStream_get(ss));
    EXPECT_EQ('\0', stringStream_get(ss));
}

static void pos(void) {
    struct D_StringStream* ss = stringStream_new("ab\ncd");
    int pos;
    int col;
    int line;
    char c;
    stringStream_getPos(ss, &pos, &col, &line);
    EXPECT_EQ(-1, pos);
    EXPECT_EQ(-1, col);
    EXPECT_EQ(1, line);
    c = stringStream_get(ss);
    EXPECT_EQ('a', c);
    stringStream_getPos(ss, &pos, &col, &line);
    EXPECT_EQ(0, pos);
    EXPECT_EQ(0, col);
    EXPECT_EQ(1, line);
    c = stringStream_get(ss);
    EXPECT_EQ('b', c);
    stringStream_getPos(ss, &pos, &col, &line);
    EXPECT_EQ(1, pos);
    EXPECT_EQ(1, col);
    EXPECT_EQ(1, line);
    c = stringStream_get(ss);
    EXPECT_EQ('\n', c);
    stringStream_getPos(ss, &pos, &col, &line);
    EXPECT_EQ(2, pos);
    EXPECT_EQ(2, col);
    EXPECT_EQ(1, line);
    c = stringStream_get(ss);
    EXPECT_EQ('c', c);
    stringStream_getPos(ss, &pos, &col, &line);
    EXPECT_EQ(3, pos);
    EXPECT_EQ(0, col);
    EXPECT_EQ(2, line);
    c = stringStream_get(ss);
    EXPECT_EQ('d', c);
    stringStream_getPos(ss, &pos, &col, &line);
    EXPECT_EQ(4, pos);
    EXPECT_EQ(1, col);
    EXPECT_EQ(2, line);
}

static void unget(void) {
    struct D_StringStream* ss = stringStream_new("ab\ncd\nef");
    int pos;
    int col;
    int line;
    EXPECT_EQ('a', stringStream_get(ss));
    EXPECT_EQ('b', stringStream_get(ss));
    EXPECT_EQ('\n', stringStream_get(ss));
    EXPECT_EQ('c', stringStream_get(ss));
    stringStream_getPos(ss, &pos, &col, &line);
    EXPECT_EQ(3, pos);
    EXPECT_EQ(0, col);
    EXPECT_EQ(2, line);
    stringStream_unget(ss);
    stringStream_getPos(ss, &pos, &col, &line);
    EXPECT_EQ(2, pos);
    EXPECT_EQ(2, col);
    EXPECT_EQ(1, line);
    stringStream_unget(ss);
    stringStream_getPos(ss, &pos, &col, &line);
    EXPECT_EQ(1, pos);
    EXPECT_EQ(1, col);
    EXPECT_EQ(1, line);
}
