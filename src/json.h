
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Code permettant de stocker différents types de couleurs : format 24 bits (RGB) ou format 32 bits (32). 
 */
#ifndef __JSON_H__
#define __JSON_H__
#include "common.h"
#include "operations.h"

char* get_code(char* data);

char* get_message(char* data);
char* get_name(char* data);
int get_calcul(char* data, struct Calc *to_calc);

int set_code(const char code[3], char* data);
int set_message(char *message, char* data);
int set_name(char* name, char* data);
int set_calcul(char* calc, char* data);
int set_colors(char* colors, char* data);

#endif // __JSON_H__