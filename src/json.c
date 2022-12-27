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
    static char code[3] = {0};
    strncpy(code, &data[strlen(FIRST_JSON_PART)], 3);
    return code;
 }

int get_calcul(char* data, struct Calc *to_calc){
    unsigned int start_index = strlen(FIRST_JSON_PART) + 3 + strlen(ARRAY_JSON_PART) + 1;
    to_calc->operation = data[start_index];
    start_index+=3;
    if(sscanf(&data[start_index], "%f,%f", &to_calc->nums[0], &to_calc->nums[1]) < 0){
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
    prepare_message(data, CODE_MSG);
    strcat(data, "\"");
    if(message[strlen(message) - 1] == '\n'){
        message[strlen(message) - 1] = '\0';
    }
    strcat(data, message);
    strcat(data, "\"]}");
    return EXIT_SUCCESS;
}

int set_name(char* name, char* data){
    prepare_message(data, CODE_NAM);
    strcat(data, "\"");
    if(name[strlen(name) - 1] == '\n'){
        name[strlen(name) - 1] = '\0';
    }
    strcat(data, name); 
    strcat(data, "\"]}");
    return EXIT_SUCCESS;
}

int set_calcul(char* calc, char* data){
    prepare_message(data, CODE_CAL);
    strcat(data, "\"");
    strncat(data, calc, 1);
    strcat(data, "\",");
    float nums[2];
    if(sscanf(&calc[1], "%f %f", &nums[0], &nums[1]) == EOF){
        printf("error parsing floats\n");
        return EXIT_FAILURE;
    }

    if(sprintf(calc, "%.2f", nums[0]) == EOF){
        return EXIT_FAILURE;
    }

    strcat(data, calc);
    strcat(data, ",");

    if(sprintf(calc, "%.2f", nums[1]) == EOF){
        return EXIT_FAILURE;
    }

    strcat(data, calc);
    strcat(data, "]}");

    return EXIT_SUCCESS;
}

void create_error_message(char* data, const char* erreur_messsage){
    prepare_message(data, CODE_ERR);
    strcat(data, "\"error: ");
    strcat(data, erreur_messsage);
    strcat(data, "\"]}");
}

void create_ok_message(char* data, const char* okk_message){
    prepare_message(data, CODE_OKK);
    strcat(data, "\"");
    strcat(data, okk_message);
    strcat(data, "\"]}");
}

void create_ok_message_int(char* data, const char* okk_message){
    prepare_message(data, CODE_OKK);
    strcat(data, okk_message);
    strcat(data, "]}");
}


void prepare_message(char* data, const char code[3]){
    strcpy(data, FIRST_JSON_PART);
    strcat(data, code);
    strcat(data, ARRAY_JSON_PART);
}

void add_element(char* data, const char* elem){
    strcat(data, ",\"");
    strcat(data, elem);
    strcat(data, "\"");
}

void add_number_element(char* data, const char* elem){
    strcat(data, ",");
    strcat(data, elem);
}

void add_first_element(char* data, const char* element){
    strcat(data, element);
}

void remove_last_newline(char* data){
    if(data[strlen(data) - 1] == '\n'){
        data[strlen(data) - 1] = '\0';
    }
}