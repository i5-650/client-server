
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* 
 * Code pour traiter différents types d'opérations mathématiques
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "operations.h"

float calculator(char *data){
  if (data[0] != '+' && data[0] != '-' && data[0] != '*' && data[0] != '/'){
    return 0.0;
  }
  float result = 0.0;
  Calc c;
  c.operator= data[0];
  if (sscanf(&data[1], "%f %f", &c.nums[0], &c.nums[1]) < 2){
    perror("Wrong input");
    return EXIT_FAILURE;
  }

  if (c.operator== '+'){
    result = c.nums[0] + c.nums[1];
  }
  else if (c.operator== '-'){
    result = c.nums[0] - c.nums[1];
  }
  else if (c.operator== '*'){
    result = c.nums[0] * c.nums[1];
  }
  else if (c.operator== '/'){
    result = c.nums[0] / c.nums[1];
  }
  else{
    return 0.0;
  }
  return result;
}

int calcul(int client_socket_fd, char *data){
  float result = calculator(data);
  if (sprintf(data, "%f", result) == EOF){
    perror("error parsing data");
    return EXIT_FAILURE;
  }

  if (write(client_socket_fd, (void *)data, strlen(data)) < 0){
    perror("error sending datas");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}