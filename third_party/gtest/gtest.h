/*
    WARNING: 这只是一个占位符，实际上并没有这个东西。
*/
#ifndef GTEST_H
#define GTEST_H

#include <stdio.h>

#define GTEST_API_ //WINAPI
namespace testing {
    void InitGoogleTest(int* pargc, char** argv);
}
void RUN_ALL_TESTS(void);

#endif
