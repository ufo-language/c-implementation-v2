#ifndef TEST_H
#define TEST_H

#include <stdlib.h>

#include "colors.h"

typedef struct {
    char* name;
    void (*test)(void);
} TestEntry;

typedef struct {
    int nPasses;
    int nFailures;
} PassFail;

extern PassFail __pf__;
extern PassFail __pfTestRun__;
extern PassFail __pfTotal__;

void runTests(char* name, void (*before)(void), TestEntry* testEntries, void (*after)(void));

#define EXPECT( __expr ) if (!(__expr)) { __pf__.nFailures++; fprintf(stderr, RED "FAILURE: %s: %s line %d\n    " #__expr "\n" CRESET, __FILE__, __func__, __LINE__); } else { __pf__.nPasses++; }

#define ASSERT( __expr ) if (!(__expr)) { __pf__.nFailures++; fprintf(stderr, RED "FAILURE: %s: %s line %d\n    expected TRUE = " #__expr "\n" CRESET, __FILE__, __func__, __LINE__); exit(1); } else { __pf__.nPasses++; }

#define EXPECT_F( __expr ) if (__expr) { __pf__.nFailures++; fprintf(stderr, RED "FAILURE: %s: %s line %d\n    expected FALSE == " #__expr "\n" CRESET, __FILE__, __func__, __LINE__); } else { __pf__.nPasses++; }

#define ASSERT_F( __expr ) if (__expr) { __pf__.nFailures++; fprintf(stderr, RED "FAILURE: %s: %s line %d\n    " #__expr "\n" CRESET, __FILE__, __func__, __LINE__); exit(1); } else { __pf__.nPasses++; }

#define EXPECT_EQ( __lhsExpr, __rhsExpr ) { long __lhs = (long)__lhsExpr; long __rhs = (long)__rhsExpr; if ((long)__lhsExpr != __rhs) { __pf__.nFailures++; fprintf(stderr, RED "FAILURE: %s: %s line %d\n    expected == %ld\n    found %ld\n" CRESET, __FILE__, __func__, __LINE__, __lhs, __rhs); } else { __pf__.nPasses++; } }

#define ASSERT_EQ( __lhsExpr, __rhsExpr ) { long __lhs = (long)__lhsExpr; long __rhs = (long)__rhsExpr; if ((long)__lhsExpr != __rhs) { __pf__.nFailures++; fprintf(stderr, RED "FAILURE: %s: %s line %d\n    expected == %ld\n    found %ld\n" CRESET, __FILE__, __func__, __LINE__, __lhs, __rhs); exit(1); } else { __pf__.nPasses++; } }

#define EXPECT_NE( __lhsExpr, __rhsExpr ) { long __lhs = (long)__lhsExpr; long __rhs = (long)__rhsExpr; if ((long)__lhsExpr == __rhs) { __pf__.nFailures++; fprintf(stderr, RED "FAILURE: %s: %s line %d\n    expected != %ld\n    found %ld\n" CRESET, __FILE__, __func__, __LINE__, __lhs, __rhs); } else { __pf__.nPasses++; } }

#define ASSERT_NE( __lhsExpr, __rhsExpr ) { long __lhs = (long)__lhsExpr; long __rhs = (long)__rhsExpr; if ((long)__lhsExpr == __rhs) { __pf__.nFailures++; fprintf(stderr, RED "FAILURE: %s: %s line %d\n    expected != %ld\n    found %ld\n" CRESET, __FILE__, __func__, __LINE__, __lhs, __rhs); exit(1); } else { __pf__.nPasses++; } }

#define EXPECT_LT( __lhsExpr, __rhsExpr ) { long __lhs = (long)__lhsExpr; long __rhs = (long)__rhsExpr; if ((long)__lhsExpr >= __rhs) { __pf__.nFailures++; fprintf(stderr, RED "FAILURE: %s: %s line %d\n    expected < %ld\n    found %ld\n" CRESET, __FILE__, __func__, __LINE__, __lhs, __rhs); } else { __pf__.nPasses++; } }

#define EXPECT_GT( __lhsExpr, __rhsExpr ) { long __lhs = (long)__lhsExpr; long __rhs = (long)__rhsExpr; if ((long)__lhsExpr <= __rhs) { __pf__.nFailures++; fprintf(stderr, RED "FAILURE: %s: %s line %d\n    expected > %ld\n    found %ld\n" CRESET, __FILE__, __func__, __LINE__, __lhs, __rhs); } else { __pf__.nPasses++; } }

#define EXPECT_NULL( __expr ) if (__expr) { __pf__.nFailures++; fprintf(stderr, RED "FAILURE: %s: %s line %d\n    expected: " #__expr " == NULL\n    found: %lx\n" CRESET, __FILE__, __func__, __LINE__, (unsigned long int)__expr); } else { __pf__.nPasses++; }

#define ASSERT_NULL( __expr ) if (__expr) { __pf__.nFailures++; fprintf(stderr, RED "FAILURE: %s: %s line %d\n    expected: " #__expr " == NULL\n    found: %lx\n" CRESET, __FILE__, __func__, __LINE__, (unsigned long int)__expr); exit(1); } else { __pf__.nPasses++; }

#define EXPECT_NOT_NULL( __expr ) if (!(__expr)) { __pf__.nFailures++; fprintf(stderr, RED "FAILURE: %s: %s line %d\n    " #__expr "\n" CRESET, __FILE__, __func__, __LINE__); } else { __pf__.nPasses++; }

#define ASSERT_NOT_NULL( __expr ) if (!(__expr)) { __pf__.nFailures++; fprintf(stderr, RED "FAILURE: %s: %s line %d\n    " #__expr "\n" CRESET, __FILE__, __func__, __LINE__); exit(1); } else { __pf__.nPasses++; }

#endif
