
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
int calcul(int client_socket_fd, char *data);
int mini(int client_socket_fd, char *data);
int maxi(int client_socket_fd, char *data);
int avg(int client_socket_fd, char *data);
int ecart(int client_socket_fd, char *data);

#endif