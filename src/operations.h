
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* 
 * Code pour traiter différents types d'opérations mathématiques
 */

#ifndef __OPERATIONS_H__
#define __OPERATIONS_H__

struct Calc {
 char operation;
 float nums[2];
};

int calculator(struct Calc *c, float* output);
int calcul(char *data);
int mini(char *data);
int maxi(char *data);
int avg(char *data);
int ecart(char *data);

#endif