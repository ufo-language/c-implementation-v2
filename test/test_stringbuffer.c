#include <string.h>

#include "test.h"

#include "../src/data/any.h"
#include "../src/data/stringbuffer.h"
#include "../src/data/stringbufferstruct.h"
#include "../src/gc/gc.h"
#include "../src/main/setup.h"

static void allocDealloc(void);
static void readWriteChar(void);
static void readWriteAcrossSegment(void);
static void chars(void);
static void charsAcrossSegment(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
    {"allocDealloc", allocDealloc},
    {"readWriteChar", readWriteChar},
    {"readWriteAcrossSegment", readWriteAcrossSegment}, 
    {"chars", chars},
    {"charsAcrossSegment", charsAcrossSegment},
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

void test_stringBuffer(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void allocDealloc(void) {
    struct D_StringBuffer* sb = stringBuffer_new();
    EXPECT_NOT_NULL(sb);
}

static void readWriteChar(void) {
    struct D_StringBuffer* sb = stringBuffer_new();
    EXPECT_EQ(0, stringBuffer_count(sb));
    stringBuffer_writeChar(sb, 'a');
    EXPECT_EQ(1, stringBuffer_count(sb));
    char c;
    bool res = stringBuffer_readChar(sb, &c);
    EXPECT(res);
    EXPECT_EQ('a', c);
    EXPECT_EQ(0, stringBuffer_count(sb));
}

static char c = 32;

static char nextChar() {
    c++;
    if (c > 126) {
        c = 33;
    }
    return c;
}

static void readWriteAcrossSegment(void) {
    struct D_StringBuffer* sb = stringBuffer_new();
    int count = 0;
    // fill the buffer
    c = 32;
    for (int n=0; n<STRINGBUFFER_SEGMENT_SIZE; n++) {
        stringBuffer_writeChar(sb, nextChar());
        count++;
        EXPECT_EQ(count, stringBuffer_count(sb));
    }
    // cross the write boundary
    stringBuffer_writeChar(sb, nextChar());
    count++;
    EXPECT_EQ(count, stringBuffer_count(sb));
    // read the whole buffer
    char c1;
    char c2;
    c = 32;
    for (int n=0; n<STRINGBUFFER_SEGMENT_SIZE; n++) {
        c1 = nextChar();
        EXPECT(stringBuffer_readChar(sb, &c2));
        EXPECT_EQ(c1, c2);
        count--;
        EXPECT_EQ(count, stringBuffer_count(sb));
    }
    // cross the read boundary
    c1 = nextChar();
    EXPECT(stringBuffer_readChar(sb, &c2));
    EXPECT_EQ(c1, c2);
    count--;
    EXPECT_EQ(count, stringBuffer_count(sb));
}

void chars(void) {
    struct D_StringBuffer* sb = stringBuffer_new();
    stringBuffer_writeChar(sb, 'a');
    stringBuffer_writeChar(sb, 'b');
    stringBuffer_writeChar(sb, 'c');
    stringBuffer_writeChar(sb, 'd');
    int count;
    char* chars = stringBuffer_getChars(sb, &count);
    EXPECT_EQ(4, count);
    EXPECT_EQ(0, strcmp("abcd", chars));
    free(chars);
}

void charsAcrossSegment(void) {
    struct D_StringBuffer* sb = stringBuffer_new();
    for (int n=0; n<STRINGBUFFER_SEGMENT_SIZE - 2; n++) {
        stringBuffer_writeChar(sb, '\0');
    }
    stringBuffer_writeChar(sb, 'a');
    stringBuffer_writeChar(sb, 'b');
    stringBuffer_writeChar(sb, 'c');
    stringBuffer_writeChar(sb, 'd');
    char c;
    for (int n=0; n<STRINGBUFFER_SEGMENT_SIZE - 2; n++) {
        stringBuffer_readChar(sb, &c);
    }
    int count;
    char* chars = stringBuffer_getChars(sb, &count);
    EXPECT_EQ(4, count);
    EXPECT_EQ(0, strcmp("abcd", chars));
    free(chars);
}
