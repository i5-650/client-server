
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

#include "json.h"
#include "operations.h"
#include "common.h"

int calculator(struct Calc *c, float* output){
  if (c->operation == '+'){
    *output = c->nums[0] + c->nums[1];
  }
  else if (c->operation == '-'){
    *output = c->nums[0] - c->nums[1];
  }
  else if (c->operation == '*'){
    *output = c->nums[0] * c->nums[1];
  }
  else if (c->operation == '/'){
    *output = c->nums[0] / c->nums[1];
  }
  else{
    return -3; // wrong operation
  }
  return EXIT_SUCCESS;
}

int calcul(int client_socket_fd, char *data){
  float result = 0.0;
  struct Calc *c = malloc(sizeof(struct Calc));
  c->operation = '%';
  c->nums[0] = 0.0;
  c->nums[1] = 0.0;
  if(get_calcul(data, c)){
    strcpy(data, FIRST_JSON_PART);
    strcat(data, CODE_ERR);
    strcat(data, ARRAY_JSON_PART);
    strcat(data, "error: parsing input\"]}");
  }

  int status = calculator(c, &result);
  char tmp[20];

  switch(status){
    case -1:
      strcpy(data, FIRST_JSON_PART);
      strcat(data, CODE_ERR);
      strcat(data, ARRAY_JSON_PART);
      strcat(data, "error: invalid operation\"]}");
      break;
    case -2:
      strcpy(data, FIRST_JSON_PART);
      strcat(data, CODE_ERR);
      strcat(data, ARRAY_JSON_PART);
      strcat(data, "error: Invalid input - couldn't parse data\"]}");
      break;
    case -3:
      strcpy(data, FIRST_JSON_PART);
      strcat(data, CODE_ERR);
      strcat(data, ARRAY_JSON_PART);
      strcat(data, "error : Impossible operation\"]}");
      break;
    default:
    printf("result %f\n", result);
      if(sprintf(tmp, "%f", result) < 0){
        strcpy(data, FIRST_JSON_PART);
        strcat(data, CODE_ERR);
        strcat(data, ARRAY_JSON_PART);
        strcpy(data, "error : Internal server error\"]}");
      }
      else {
        strcpy(data, FIRST_JSON_PART);
        strcat(data, CODE_CAL);
        strcat(data, ARRAY_JSON_PART);
        strcat(data, "\"");
        strcat(data, tmp);
        strcat(data, "\"]}");
      }
  }
  free(c);

  if(write(client_socket_fd, (void *)data, strlen(data)) < 0){
    perror("error sending datas");
    return EXIT_FAILURE;
  } 

  return EXIT_SUCCESS;
}