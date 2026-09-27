/*
    WARNING: 这只是一个占位符，实际上并没有这个东西。
*/

#include "gtest.h"
#include <cstddef>
#include <cstdio>

namespace testing {
    void InitGoogleTest(int* pargc, char** argv) {
        if ((pargc != NULL) && (argv != NULL)) {
            if (*pargc > 1) {
                printf("%s", argv[1]);
            }
        }
    }
}

void RUN_ALL_TESTS(void) {}
