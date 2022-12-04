
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* 
 * Le but principal de ce code est de s'assurer que les messages envoyés par le client sont
 * valides, c'est-à-dire que les messages respectent le format JSON et
 * respectent le protocole spécifié par le côté serveur.
 */

 #include "validation.h"
#include "common.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

int check_array_content(char* data){
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
    //printf("%s\n", &data[start_index]);
    if(check_array_content(&data[start_index])){
        return EXIT_FAILURE;
    }

    // last char
    if(data[strlen(data)-1] != '}' || data[strlen(data)-2] != ']'){
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

const char *skip_whitespace(const char *str){
    while (*str == ' ' || *str == '\t' || *str == '\r' || *str == '\n') {
        str++;
    }
    return str;
}

int is_digit(char ch){
    return ch >= '0' && ch <= '9';
}

const char *parse_string(const char *str){
    str = skip_whitespace(str);
    // String must begin with a double quote
    if (*str++ != '"') {
        return NULL;
    }

    // Parse characters until the closing double quote
    while (*str != '"') {
        if (*str == '\0') {
            // End of string without closing double quote
            return NULL;
        }

        if (*str == '\\') {
            // Escape sequence
            str++;
            if (*str == '\0') {
                // End of string after escape character
                return NULL;
            }

            // Skip over the escaped character
            str++;
        } else {
            // Regular character
            str++;
        }
    }

    // Skip over the closing double quote
    str++;

    return str;
}

const char *parse_number(const char *str){
    if (*str == '-') {
        // Optional minus sign
        str++;
    }

    // Parse digits before the decimal point
    if (*str == '0') {
        // Zero must not be followed by a decimal point or digit
        str++;
        if (*str == '.' || is_digit(*str)) {
            return NULL;
        }
    } else {
        // Parse non-zero digits before the decimal point
        while (is_digit(*str)) {
            str++;
        }
    }

    // Parse digits after the decimal point
    if (*str == '.') {
        str++;

        // Decimal point must be followed by at least one digit
        if (!is_digit(*str)) {
            return NULL;
        }

        // Parse digits after the decimal point
        while (is_digit(*str)) {
            str++;
        }
    }

    // Parse exponent
    if (*str == 'e' || *str == 'E') {
        str++;

        // Exponent must be followed by a plus or minus sign
        if (*str == '+' || *str == '-') {
            str++;
        }

        // Exponent must be followed by at least one digit
        if (!is_digit(*str)) {
            return NULL;
        }

        // Parse digits in the exponent
        while (is_digit(*str)) {
            str++;
        }
    }

    // Return the pointer to the next character after the number
    return str;
}

const char *parse_value(const char *str){
    // Skip over whitespace
    str = skip_whitespace(str);

    // Check for the beginning of an object or array
    if (*str == '[') {
        // Parse an array
        str++;

        // Parse values in the array until the closing bracket is found
        while (*str != ']') {
            str = parse_value(str);
            if (str == NULL) {
                return NULL;
            }

            // Skip over the comma between values, if present
            str = skip_whitespace(str);
            if (*str == ',') {
                str++;
            }
        }

        // Skip over the closing bracket
        str++;
    } else if (*str == '{') {
        // Parse an object
        str++;

        // Parse key-value pairs in the object until the closing bracket is found
        while (*str != '}') {
            // Parse the key
            str = parse_string(str);
            if (str == NULL) {
                return NULL;
            }

            // Skip over the colon between the key and the value
            str = skip_whitespace(str);
            if (*str != ':') {
                return NULL;
            }
            str++;

            // Parse the value
            str = parse_value(str);
            if (str == NULL) {
                return NULL;
            }

            // Skip over the comma between key-value pairs, if present
            str = skip_whitespace(str);
            if (*str == ',') {
                str++;
            }
        }

        // Skip over the closing bracket
        str++;
    } else if (*str == '"') {
        // Parse a string
        str = parse_string(str);
        if (str == NULL) {
            return NULL;
        }
    } else if (*str == '-' || is_digit(*str)) {
        // Parse a number
        str = parse_number(str);
        if (str == NULL) {
            return NULL;
        }
    } else if (strncmp(str, "true", 4) == 0) {
        // Parse "true"
        str += 4;
    } else if (strncmp(str, "false", 5) == 0) {
        // Parse "false"
        str += 5;
    } else if (strncmp(str, "null", 4) == 0) {
        // Parse "null"
        str += 4;
    } else {
        // Invalid character
        return NULL;
    }

    // Return the pointer to the next character after the value
    return str;
}


int json_validator(const char *data){
    const char *str = data;

    // Parse the JSON string
    while (*str != '\0') {
        // Skip over whitespace
        str = skip_whitespace(str);

        // Check for the beginning of an object or array
        if (*str == '[' || *str == '{') {
            // Parse the object or array recursively
            str = parse_value(str);
            if (str == NULL) {
                return 0;
            }
        } else if (*str == '"') {
            // Parse a string
            str = parse_string(str);
            if (str == NULL) {
                return 0;
            }
        } else if (*str == '-' || is_digit(*str)) {
            // Parse a number
            str = parse_number(str);
            if (str == NULL) {
                return 0;
            }
        } else if (strncmp(str, "true", 4) == 0) {
            // Parse "true"
            str += 4;
        } else if (strncmp(str, "false", 5) == 0) {
            // Parse "false"
            str += 5;
        } else if (strncmp(str, "null", 4) == 0) {
            // Parse "null"
            str += 4;
        } else {
            // Invalid character
            return 0;
        }
    }

    // Make sure the string has no remaining characters
    str = skip_whitespace(str);
    if (*str != '\0') {
        return 0;
    }

    // JSON string is valid
    return 1;
}