#include "json_parser.h"
#include "common.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int check_array_content(char* data){
    int quote = 0;
    for(size_t i = 0; i < strlen(data) - 2; i++){
        if(data[i] == '"'){
            ++i;
            while(data[i] != '"'){
                if(data[i] == ',' || data[i] == '}'){
                    return EXIT_FAILURE;
                }
                ++i;
            }
        }
        if(data[i] == ',' && data[i+1] != '"'){
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int validate_json(char* data){
    unsigned int start_index = 0;
    // check if the minimal length is reached
    if(strlen(data) < (strlen(FIRST_JSON_PART) + CODE_LEN + strlen(ARRAY_JSON_PART))){
        return EXIT_FAILURE;
    }

    // matching first part
    if(strncmp(data, FIRST_JSON_PART, strlen(FIRST_JSON_PART))){
        return EXIT_FAILURE;
    }
    start_index+= strlen(FIRST_JSON_PART);

    // skip code -> len + 3
    start_index+=3;

    if(strncmp(&data[start_index], ARRAY_JSON_PART, strlen(ARRAY_JSON_PART))){
        return EXIT_FAILURE;
    }

    // check array content
    start_index+=strlen(ARRAY_JSON_PART);
    printf("%s\n", &data[start_index]);
    if(check_array_content(&data[start_index])){
        return EXIT_FAILURE;
    }

    // last char
    if(data[strlen(data)-1] != '}' || data[strlen(data)-2] != ']'){
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]){
    char* data = "{\"code\":\"msg\",\"values\":[\"truc\",\"machine\"]}";
    printf("%s\n", data);
    if(validate_json(data)){
        printf("pas bon\n");
    }
    else{
        printf("bon\n");
    }
    return 0;
}

/*
char* get_code(char* data);

char* get_message(char* data);
char* get_name(char* data);
char* get_calcul(char* data);
char* get_colors(char* data);
char* get_tags(char* data);
*/