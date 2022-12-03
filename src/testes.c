#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "operations.h"
#include "testes.h"
#include "validation.h"

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
    ASSERT_EQ_CHAR('A', 'B');
    ASSERT_EQ_CODE(CODE_MSG, "lol");
    ASSERT_EQ_STR("lol", "olo");
    ASSERT_EQ_INT(1, 2);
    ASSERT_EQ_FLOAT(1.0, 2.0);
    return 0;
}

