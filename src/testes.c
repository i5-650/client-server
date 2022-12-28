#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "operations.h"
#include "testes.h"
#include "validation.h"
#include "client.h"

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


int main(){
    char *data = "{\"code\": 1, \"data\": [1, 2, 3, 4, 5]}";
    char *invalid_json = "{\"code\": 1, \"data\": [1, 2, 3, 4, 5";
    ASSERT_EQ_INT(1, json_validator(data));
    ASSERT_EQ_INT(1, json_validator(INVALID_JSON_MSG));
    ASSERT_EQ_INT(0, json_validator(invalid_json));
    char temp_data[1024];
    ASSERT_EQ_INT(EXIT_FAILURE, envoie_recois_name(temp_data));
    return 0;
}

