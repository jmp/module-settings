/*
 * This is a set of macros/functions for testing small C programs.
 * It's nothing fancy, but good enough for my own purposes.
 * To start writing some tests, simply include this header file.
 *
 * Optionally, the standard malloc and realloc can be wrapped with a
 * version that fails after the given number of calls. This requires
 * the GCC flag "-DWRAP_MALLOC -Wl,--wrap,malloc" (and similar for
 * realloc). It can be used to test memory allocation failures,
 * without having to touch the code being tested.
 */

#ifndef TEST_H
#define TEST_H

/* Constants */
#define TEST_PASS 1
#define TEST_FAIL 0

#include <stdio.h>
#include <stdlib.h>

/*
 * Assert that a condition is true.
 *
 * Causes the function to return TEST_FAIL if the assertion fails.
 * Otherwise, nothing happens. The macro also records the expression that
 * failed, as well as the file and line number where the assertion failed.
 */
#define test_assert(expr) \
	if (!(expr)) { \
		test_fail_expr = #expr; \
		test_fail_file = __FILE__; \
		test_fail_line = __LINE__; \
		return TEST_FAIL; \
	}

/*
 * Run the given test function and print the results for that test.
 *
 * This will print either "OK" or "FAIL", followed by the name of the test.
 * If the test fails, the filename and line number as well as the expression
 * that failed will be printed.
 */
#define test_run(test_name) { \
		test_malloc_enable(); \
		test_realloc_enable(); \
		test_fail_expr = NULL; \
		test_fail_file = NULL; \
		test_fail_line = 0L; \
		if ((test_name)() == TEST_PASS) { \
			++test_pass_count; \
			fprintf(stdout, "OK    " #test_name "\n"); \
		} else { \
			++test_fail_count; \
			fprintf(stderr, "FAIL  " #test_name "\n"); \
			if (test_fail_expr != NULL) { \
				fprintf(stderr, "\n      In file %s, line %ld:\n\n", \
					test_fail_file, test_fail_line); \
				fprintf(stderr, "          %s\n\n", test_fail_expr); \
				fprintf(stderr, "      Assertion of the above expression" \
					" failed.\n\n"); \
			} \
		} \
	}

/* Print the total/passed/failed test counts. */
#define test_print_stats() \
	printf( \
		"\n%d total, %d passed, %d failed.\n", \
		test_pass_count + test_fail_count, \
		test_pass_count, \
		test_fail_count \
	);

/* Gets the number of failed tests. */
#define test_get_fail_count() (test_fail_count)

/* Keep track of how many tests have passed and failed. */
unsigned test_pass_count = 0;
unsigned test_fail_count = 0;

/* The latest expression, file and line where the assertion failed. */
const char *test_fail_expr = NULL;
const char *test_fail_file = NULL;
long test_fail_line = 0L;

/*
 * Optionally, wrap malloc with a custom one that fails after
 * a specified number of calls. The number of calls is defined
 * by the test_malloc_fail_after variable.
 *
 * Using GCC, the program must be compiled with "-Wl,--wrap,malloc",
 * which makes all malloc calls use __wrap_malloc instead.
 */
#ifdef WRAP_MALLOC

/* Counts the number of malloc calls made so far. */
long test_malloc_call_count = 0L;

/* After how many calls malloc will fail (return NULL). */
long test_malloc_fail_after = -1;

extern void *__real_malloc(size_t s);

void *__wrap_malloc(size_t s)
{
	if (test_malloc_call_count++ < test_malloc_fail_after ||
		test_malloc_fail_after < 0) {
		return __real_malloc(s);
	}
	return NULL;
}

/* Makes malloc return NULL after the given number of calls. */
#define test_malloc_fail_after(n) { \
		test_malloc_call_count = 0; \
		test_malloc_fail_after = (n); \
	}

/* Convenience macros for making malloc fail/succeed. */
#define test_malloc_enable() test_malloc_fail_after(-1)
#define test_malloc_disable() test_malloc_fail_after(0)

#else

#define test_malloc_fail_after(n) do {} while (0)
#define test_malloc_enable() do {} while (0)
#define test_malloc_disable() do {} while (0)

#endif /* WRAP_MALLOC */

/*
 * Optionally, wrap realloc with a custom one that fails after
 * a specified number of calls. The number of calls is defined
 * by the test_realloc_fail_after variable.
 *
 * Using GCC, the program must be compiled with "-Wl,--wrap,realloc",
 * which makes all realloc calls use __wrap_realloc instead.
 */
#ifdef WRAP_REALLOC

/* Counts the number of realloc calls made so far. */
long test_realloc_call_count = 0;

/*
 * After how many calls realloc will fail (return NULL).
 * If set to negative, realloc performs normally.
 */
long test_realloc_fail_after = -1;

extern void *__real_realloc(void *ptr, size_t s);

void *__wrap_realloc(void *ptr, size_t s)
{
	if (test_realloc_call_count++ < test_realloc_fail_after ||
		test_realloc_fail_after < 0) {
		return __real_realloc(ptr, s);
	}
	return NULL;
}

/* Makes realloc return NULL after the given number of calls. */
#define test_realloc_fail_after(n) { \
		test_realloc_call_count = 0; \
		test_realloc_fail_after = (n); \
	}

/* Convenience macros for making realloc fail/succeed. */
#define test_realloc_enable() test_realloc_fail_after(-1)
#define test_realloc_disable() test_realloc_fail_after(0)

#else

#define test_realloc_fail_after(n) do {} while (0)
#define test_realloc_enable() do {} while (0)
#define test_realloc_disable() do {} while (0)

#endif /* WRAP_REALLOC */

#endif /* TEST_H */
