
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
#include "common.h"

int calculator(char *data, float* output){
  printf("%s\n", data);
  unsigned int starting_index = strlen(HEADER_CALCUL);
  if (data[starting_index] != '+' 
    && data[starting_index] != '-' 
    && data[starting_index] != '*' 
    && data[starting_index] != '/'){
    return -1; // wrong operation
  }
  struct Calc c;
  c.operation= data[starting_index];
  if (sscanf(&data[starting_index + 1], "%f %f", &c.nums[0], &c.nums[1]) < 2){
    perror("Wrong input");
    return -2; // couldn't parse
  }

  if (c.operation== '+'){
    *output = c.nums[0] + c.nums[1];
  }
  else if (c.operation== '-'){
    *output = c.nums[0] - c.nums[1];
  }
  else if (c.operation== '*'){
    *output = c.nums[0] * c.nums[1];
  }
  else if (c.operation== '/'){
    *output = c.nums[0] / c.nums[1];
  }
  else{
    return -3; // wrong operation
  }
  return EXIT_SUCCESS;
}

int calcul(int client_socket_fd, char *data){
  float result = 0.0;
  int status = calculator(data, &result);

  switch(status){
    case -1:
      strcpy(data, "error : Invalid operator");
      break;
    case -2:
      strcpy(data, "error : Invalid input - couldn't parse data");
      break;
    case -3:
      strcpy(data, "error : Impossible operation");
      break;
    default:
      if (sprintf(data, "%f", result) == EOF){
        strcpy(data, "error : Internal server error");
      }
  }

  if(write(client_socket_fd, (void *)data, strlen(data)) < 0){
    perror("error sending datas");
    return EXIT_FAILURE;
  } 

  return EXIT_SUCCESS;
}