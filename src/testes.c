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
    char *str = "[\"machine\",\"bidule\",\"ta_darone_la_pute\"]";
    char out[3][1024];
    
    if(!sscanf(str, "[\"%s\",\"%s\",\"%s\"]", out[0], out[1], out[2])){
        printf("TA MERE LA PUTE\n");
        return EXIT_FAILURE;
    }
    printf("%s\n%s\n%s\n", out[0], out[1], out[2]);
    return 0;
}

