#include <stdio.h>

#include "colors.h"
#include "test.h"

void test_abstraction(void);
void test_apply(void);
void test_array(void);
void test_boolean(void);
void test_do(void);
void test_evaluator(void);
void test_gc1(void);
void test_gc2(void);
void test_hashTable(void);
void test_identifier(void);
void test_if(void);
void test_integer(void);
void test_lexer(void);
void test_nothing(void);
void test_parser(void);
void test_queue(void);
void test_sizeOf(void);
void test_string(void);
void test_stringBuffer(void);
void test_stringStream(void);
void test_symbol(void);

static void (*tests[])(void) = {
    test_boolean,
    test_integer,
    test_nothing,
    test_gc1,
    test_gc2,
    test_array,
    test_hashTable,
    test_if,
    test_do,
    test_abstraction,
    test_apply,
    test_queue,
    test_stringStream,
    test_stringBuffer,
    test_string,
    test_sizeOf,
    test_evaluator,
    test_hashTable,
    test_symbol,
    test_identifier,
    test_lexer,
    test_parser,
    0
};

 
void alltests(void) {
    fprintf(stderr, "/ %s started\n\n", __func__);
    for (int n=0; *tests[n]; n++) {
        __pfTestRun__.nPasses = 0;
        __pfTestRun__.nFailures = 0;
        tests[n]();
        __pfTotal__.nPasses += __pfTestRun__.nPasses;
        __pfTotal__.nFailures += __pfTestRun__.nFailures;
        fputc('\n', stderr);
    }
    if (__pfTotal__.nFailures == 0) {
        fputs(GRN, stderr);
    }
    else {
        fputs(YEL, stderr);
    }
    fprintf(stderr, "\\ total tests passed: %d/%d (%d failures)\n", __pfTotal__.nPasses, __pfTotal__.nPasses + __pfTotal__.nFailures, __pfTotal__.nFailures);
    fputs(CRESET, stderr);
}

PassFail __pf__;
PassFail __pfTestRun__;
PassFail __pfTotal__;

void runTests(char* name, void (*before)(void), TestEntry* testEntries, void (*after)(void)) {
    fprintf(stderr, "/ %s started\n", name);

    while (1) {
        TestEntry testEntry = *testEntries;
        char* name = testEntry.name;
        if (!name) {
            break;
        }
        void (*test)(void) = testEntry.test;
        fprintf(stderr, "| / %s started\n", name);
        __pf__.nPasses = 0;
        __pf__.nFailures = 0;
        before();
        test();
        after();
        if (__pf__.nPasses + __pf__.nFailures == 0) {
            fputs(YEL "WARNING no tests were performed\n" CRESET, stderr);
        }
        
        if (__pf__.nFailures > 0) {
            fputs(YEL, stderr);
        }
        fprintf(stderr, "| \\ %s tests passed: %d/%d\n" CRESET, name, __pf__.nPasses, __pf__.nPasses + __pf__.nFailures);
        fputs(CRESET, stderr);
        __pfTestRun__.nPasses += __pf__.nPasses;
        __pfTestRun__.nFailures += __pf__.nFailures;
        testEntries++;
    }
  
    fputs(__pfTestRun__.nFailures == 0 ? GRN : YEL, stderr);
    fprintf(stderr, "\\ %s tests passed: %d/%d\n", name, __pfTestRun__.nPasses, __pfTestRun__.nPasses + __pfTestRun__.nFailures);
    fputs(CRESET, stderr);
}
