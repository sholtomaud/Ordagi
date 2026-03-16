/* tests/test_runner.h */
#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <stdio.h>
#include <math.h>
#include <string.h>

extern int _tests_run;
extern int _tests_passed;
extern int _tests_failed;

#define ASSERT_TRUE(expr) do { \
    _tests_run++; \
    if (!(expr)) { \
        fprintf(stderr, "FAIL: %s:%d: ASSERT_TRUE(%s)\n", \
                __FILE__, __LINE__, #expr); \
        _tests_failed++; \
    } else { _tests_passed++; } \
} while(0)

#define ASSERT_FALSE(expr)        ASSERT_TRUE(!(expr))
#define ASSERT_INT_EQ(a, b)       ASSERT_TRUE((a) == (b))
#define ASSERT_STR_EQ(a, b)       ASSERT_TRUE(strcmp((a),(b)) == 0)
#define ASSERT_DOUBLE_EQ(a, b, eps) ASSERT_TRUE(fabs((a)-(b)) < (eps))
#define ASSERT_NOT_NULL(ptr)      ASSERT_TRUE((ptr) != NULL)
#define ASSERT_NULL(ptr)          ASSERT_TRUE((ptr) == NULL)

#define RUN_TEST(fn) do { \
    printf("  running %-50s", #fn); \
    fn(); \
    printf("ok\n"); \
} while(0)

#define PRINT_TEST_SUMMARY() do { \
    printf("\n%d tests: %d passed, %d failed\n", \
           _tests_run, _tests_passed, _tests_failed); \
} while(0)

#endif /* TEST_RUNNER_H */
