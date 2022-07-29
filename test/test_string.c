#include <string.h>

#include "test.h"

#include "../src/data/any.h"
#include "../src/data/list.h"
#include "../src/data/string.h"
#include "../src/gc/gc.h"
#include "../src/main/globals.h"
#include "../src/main/setup.h"

static void allocDealloc(void);
static void split0(void);
static void split1(void);
static void split3(void);

// List the unit tests to run here -----------------------------------

static TestEntry testEntries[] = {
    {"allocDealloc", allocDealloc},
    {"split0", split0},
    {"split1", split1},
    {"split3", split3},
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

void test_string(void) {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

// Unit tests --------------------------------------------------------

static void allocDealloc(void) {
    char* chars = "Abc Def";
    struct D_String* string = string_new(chars);
    EXPECT_NOT_NULL(string);
}

static void split0(void) {
    struct D_List* tokens = string_split(EMPTY_STRING, ' ' );
    EXPECT_EQ(1, list_count(tokens));
    struct D_String* token = (struct D_String*)list_getFirst(tokens);
    EXPECT(0 == strcmp("", string_getChars(token)));
}

static void split1(void) {
    char* chars = "Abc";
    struct D_String* string = string_new(chars);
    struct D_List* tokens = string_split(string, ' ' );
    EXPECT_EQ(1, list_count(tokens));
    struct D_String* token = (struct D_String*)list_getFirst(tokens);
    EXPECT(0 == strcmp("Abc", string_getChars(token)));
}

static void split3(void) {
    char* chars = "Abc Def Ghi";
    struct D_String* string = string_new(chars);
    struct D_List* tokens = string_split(string, ' ' );
    EXPECT_EQ(3, list_count(tokens));
    struct D_String* token = (struct D_String*)list_getFirst(tokens);
    EXPECT(0 == strcmp("Abc", string_getChars(token)));
    tokens = (struct D_List*)list_getRest(tokens);
    token = (struct D_String*)list_getFirst(tokens);
    EXPECT(0 == strcmp("Def", string_getChars(token)));
    tokens = (struct D_List*)list_getRest(tokens);
    token = (struct D_String*)list_getFirst(tokens);
    EXPECT(0 == strcmp("Ghi", string_getChars(token)));
    tokens = (struct D_List*)list_getRest(tokens);
    EXPECT_EQ(EMPTY_LIST, tokens);
}
