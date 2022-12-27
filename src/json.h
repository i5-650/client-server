
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

void remove_last_newline(char* data);

char* get_code(char* data);
int get_calcul(char* data, struct Calc *to_calc);

int set_code(const char code[3], char* data);
int set_message(char *message, char* data);
int set_name(char* name, char* data);
int set_calcul(char* calc, char* data);

void create_error_message(char* data, const char* erreur_messsage);
void create_ok_message(char* data, const char* okk_message);
void create_ok_message_int(char* data, const char* okk_message);
void prepare_message(char* data, const char code[3]);
void add_element(char* data, const char* element);
void add_first_element(char* data, const char* element);
void add_number_element(char* data, const char* elem);

#endif // __JSON_H__