#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "operations.h"
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

int main(int argc, char* argv[]){

    printf("Expected: 10, real: %f\n", calculator("+ 5 5"));
    printf("Expected: 12.3, real: %f\n", calculator("* 4.1 3"));

    return 0;
}

