/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
/*
 * Code pour le traitement des messages au format JSON
 */

#include "json.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

 char* get_code(char* data){
    char *code = malloc(sizeof(char) * 3);
    strncpy(code, &data[strlen(FIRST_JSON_PART)], 3);
    return code;
 }

char* get_word_till_quote(char* data){
    unsigned int end_index = 0;
    for(size_t i = 0; i < strlen(data); ++i){
        if(data[i] == '"'){
            end_index = i;
            break;
        }
    }
    data[end_index + 1] = '\0';
    free(&data[end_index + 2]);
    return data;
}

int get_calcul(char* data, struct Calc *to_calc){
    unsigned int start_index = strlen(FIRST_JSON_PART) + 3 + strlen(ARRAY_JSON_PART) + 1;
    to_calc->operation = data[start_index];
    start_index+=4;
    if(sscanf(&data[start_index], "%f\",\"%f", &to_calc->nums[0], &to_calc->nums[1]) == EOF){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int set_code(const char code[3], char* data){
    strcat(data, code);
    strcat(data, ARRAY_JSON_PART);
    return EXIT_SUCCESS;
}

int set_message(char *message, char* data){
    strcat(data, "\"");
    printf("char: %c", message[strlen(message) -1]);
    if(message[strlen(message) - 1] == '\n'){
        message[strlen(message) - 1] = '\0';
    }
    strcat(data, message);
    strcat(data, "\"]}");
    return EXIT_SUCCESS;
}

int set_name(char* name, char* data){
    strcat(data, "\"");
    if(name[strlen(name) - 1] == '\n'){
        name[strlen(name) - 1] = '\0';
    }
    strcat(data, name); 
    strcat(data, "\"]}");
    return EXIT_SUCCESS;
}

int set_calcul(char* calc, char* data){
    strcat(data, "\"");
    strncat(data, calc, 1);
    strcat(data, "\",\"");
    float nums[2];
    if(sscanf(&calc[1], "%f %f", &nums[0], &nums[1]) == EOF){
        printf("error parsing floats\n");
        return EXIT_FAILURE;
    }

    if(sprintf(calc, "%.2f", nums[0]) == EOF){
        return EXIT_FAILURE;
    }

    strcat(data, calc);
    strcat(data, "\",\"");

    if(sprintf(calc, "%.2f", nums[1]) == EOF){
        return EXIT_FAILURE;
    }

    strcat(data, calc);
    strcat(data, "\"]}");

    return EXIT_SUCCESS;
}