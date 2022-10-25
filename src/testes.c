#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

struct Calc {
 char operator;
 float nums[2];
}Calc;

float calculator(char* data){
    if(data[0] != '+' && data[0] != '-' && data[0] != '*' && data[0] != '%' && data[0] != '/'){
        return 0.0;
    }
    printf("%s\n", data);
    char* str;
    float result = 0.0;
    struct Calc c;
    c.operator = data[0];
    c.nums[0] = strtof(&data[1], &str);
    c.nums[1] = strtof(str, NULL);

    if(c.operator == '+'){
      result = c.nums[0] + c.nums[1];
    }
    else if(c.operator == '-'){
      result = c.nums[0] - c.nums[1];
    }
    else if(c.operator == '*'){
      result = c.nums[0] * c.nums[1];
    }
    else if(c.operator == '/'){
      result = c.nums[0] / c.nums[1];
    }
    else {
      return 0.0;
    }
    return result;
}

int main(int argc, char* argv[]){

    printf("Expected: 10, real: %f\n", calculator("+ 5 5"));
    printf("Expected: 12.3, real: %f\n", calculator("* 4.1 3"));

    return 0;
}

