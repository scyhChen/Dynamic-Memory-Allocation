#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "heaplib.h"
#include "ctests.c"

int main(int argc, char *argv[]) {

    if (argc <= 1) {
        printf("Usage: %s <test #>\n", argv[0]);
        exit(1);
    }

    int (*tests[])(void) = { test01, test02, test03, test04, test05, test06, test07, test08, test09, test10, test11, test12, test12, test14, test15, test16 };
    int num_tests = sizeof(tests) / sizeof(int*);

    int test_num = atoi(argv[1]); // test 1 located at index 0
    int test_index = test_num - 1; // test 1 located at index 0

    if (test_index < 0 || test_index >= num_tests) {
        printf("Invalid test # not in (1-%d): %d\n", num_tests, test_num);
        exit(1);
    }

    assert(tests[test_index]()); // Should succeed

    printf("Test %d (%s) passed\n", test_num, testDescriptions[test_index]);

    return 0;
}
