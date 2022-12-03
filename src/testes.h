
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* 
 * L'objectif principal de ce code est d'effectuer des tests unitaires et
 *  fonctionnels sur les différents composants créés dans ce projet.
 */

#include <stdio.h>
#include <string.h>
#ifndef TESTES_H
#define TESTES_H

#define ASSERT_EQ_INT(exp, act) assertEqualsInt(exp, act, __func__)
#define ASSERT_EQ_STR(exp, act) assertEqualsString(exp, act, __func__)
#define ASSERT_EQ_FLOAT(exp, act) assertEqualsFloat(exp, act, __func__)
#define ASSERT_EQ_CHAR(exp, act) assertEqualsChar(exp, act, __func__)
#define ASSERT_EQ_CODE(exp, act) assertEqualsCode(exp, act, __func__)

// Useful functions for testing

void assertEqualsInt(int expected, int actual, const char* caller){
    if(expected != actual){
        printf("Function: %s\n\tExpected: %d\n\tActual: %d\n", caller, expected, actual);
    }
}

void assertEqualsString(char* expected, char* actual, const char* caller){
    if(strcmp(expected, actual) != 0){
        printf("Function: %s\n\tExpected: %s\n\tActual: %s\n", caller, expected, actual);
    }
}

void assertEqualsChar(char expected, char actual, const char* caller){
    if(expected != actual){
        printf("Function: %s\n\tExpected: %c\n\tActual: %c\n", caller, expected, actual);
    }
}

void assertEqualsFloat(float expected, float actual, const char* caller){
    if(expected != actual){
        printf("Function: %s\n\tExpected: %f\n\tActual: %f\n", caller, expected, actual);
    }
}

void assertEqualsCode(char* expected, char* actual, const char* caller){
    if(strncmp(expected, actual, 3) != 0){
        printf("Function: %s\n\tExpected: %s\n\tActual: %s\n", caller, expected, actual);
    }
}

#endif // TESTES_H