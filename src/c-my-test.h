#include <stdio.h>
#include <string.h>
#ifndef _C_MY_TEST_H_
#define _C_MY_TEST_H_

#define ASSERT_EQ_INT(exp, act) assertEqualsInt(exp, act, __func__)
#define ASSERT_EQ_STR(exp, act) assertEqualsString(exp, act, __func__)
#define ASSERT_EQ_FLOAT(exp, act) assertEqualsFloat(exp, act, __func__)
#define ASSERT_EQ_CHAR(exp, act) assertEqualsChar(exp, act, __func__)
#define ASSERT_EQ_CODE(exp, act) assertEqualsCode(exp, act, __func__)
#define ASSERT_EQ_DOUBLE(exp, act) assertEqualsDouble(exp, act, __func__)

int failed = 0;
int total = 0;

// Useful functions for testing

void assertEqualsInt(int expected, int actual, const char* caller){
    total++;
    if(expected != actual){
        failed++;
        printf("Function: %s\n\tExpected: %d\n\tActual: %d\n", caller, expected, actual);
    }
}

void assertEqualsString(char* expected, char* actual, const char* caller){
    total++;
    if(strcmp(expected, actual) != 0){
        failed++;
        printf("Function: %s\n\tExpected: %s\n\tActual: %s\n", caller, expected, actual);
    }
}

void assertEqualsChar(char expected, char actual, const char* caller){
    total++;
    if(expected != actual){
        failed++;
        printf("Function: %s\n\tExpected: %c\n\tActual: %c\n", caller, expected, actual);
    }
}

void assertEqualsFloat(float expected, float actual, const char* caller){
    total++;
    if(expected != actual){
        failed++;
        printf("Function: %s\n\tExpected: %f\n\tActual: %f\n", caller, expected, actual);
    }
}

void assertEqualsCode(char* expected, char* actual, const char* caller){
    total++;
    if(strncmp(expected, actual, 3) != 0){
        failed++;
        printf("Function: %s\n\tExpected: %s\n\tActual: %s\n", caller, expected, actual);
    }
}

void assertEqualsDouble(double exp, double act, const char* caller){
    total++;
    if(exp != act){
        failed++;
        printf("Function: %s\n\tExpected: %f\n\tActual: %f\n", caller, exp, act);
    }
}

void end_test(){
    printf("Total: %d\nFailed: %d\nPassed %d\n", total, failed, (total - failed));
}

#endif // _C_MY_TEST_H_