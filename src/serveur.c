/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Le code côté serveur. L'objectif principal est de recevoir des messages des clients,
 * de traiter ces messages et de répondre aux clients.
 */

#include "serveur.h"
#include "operations.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "json.h"
#include "validation.h"

int main(){
  int socketfd;
  int bind_status;
  struct sockaddr_in server_addr;

  //Creation d'une socket
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0){
    perror("[/!\\] Unable to open a socket");
    return -1;
  }
  printf("[+] Socket created\n");

  int option = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  // détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Relier l'adresse à la socket
  bind_status = bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (bind_status < 0){
    perror("bind");
    return (EXIT_FAILURE);
  }
  printf("[+] Socket binded\n");

  // Écouter les messages envoyés par le client
  listen(socketfd, 10);
  struct sockaddr_in client_addr;

  unsigned int client_addr_len = sizeof(client_addr);

  // nouvelle connection de client
  int client_socket_fd = accept(socketfd, (struct sockaddr *)&client_addr, &client_addr_len);
  printf("[+] New client connected !\n");
  
  if(recois_envoie_message(client_socket_fd) < 0){
    perror("[/!\\] Error during first connection: ");
  }

  while (1){
    recois_envoie_message(client_socket_fd);
  }

  return 0;
}

void plot(char *data){
  // Extraire le compteur et les couleurs RGB
  FILE *p = fopen("gnuplot.txt", "a");
  int count = 0;
  int n;

  char *str = data;

  fprintf(p, "set xrange [-15:15]\n");
  fprintf(p, "set yrange [-15:15]\n");
  fprintf(p, "set style fill transparent solid 0.9 noborder\n");
  fprintf(p, "set title 'Top 10 colors'\n");
  fprintf(p, "plot '-' with circles lc rgbcolor variable\n");
  unsigned int start_delimiter = strlen(FIRST_JSON_PART) + 3 + strlen(ARRAY_JSON_PART);
  memset(&str[strlen(str) - 2], 0, sizeof(char)*2);
  while (1){
    char *token = strtok(&str[start_delimiter], ",");
    if (token == NULL){
      break;
    }

    if (count == 0){
      sscanf(token,"\"%d\"",&n);
    }
    else{
      // Le numéro 36, parceque 360° (cercle) / 10 couleurs = 36
      fprintf(p, "0 0 10 %d %d 0x%s\n", (count - 1) * 36, count * 36, token + 1);
    }
    count++;
    start_delimiter+=strlen(token)+1;
  }

  fprintf(p, "e\n");
  fclose(p);
}

/* renvoyer un message (*data) au client (client_socket_fd)
 */
int renvoie_message(int client_socket_fd, char *data){
  int data_size = write(client_socket_fd, (void *)data, strlen(data));

  if (data_size < 0){
    perror("[/!\\] erreur ecriture");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int renvoie_name(int client_socket_fd, char *data){
  int status = renvoie_message(client_socket_fd, data);
  return status;
}

/* accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(int client_socket_fd){
  char data[1024];
  memset(data, 0, sizeof(char)*DATA_LEN);

  int data_size = read(client_socket_fd, (void *)data, sizeof(char)*DATA_LEN);
  if (data_size < 0){
    perror("erreur lecture");
    return EXIT_FAILURE;
  }

  if(validate_json(data)){
    perror("[/!\\] Invalid JSON");
    return EXIT_FAILURE;
  }

  printf("[+] Données recus: %s\n", data);
  char *code = get_code(data);

  if(!strncmp(code, CODE_MSG, 3)){
    if(renvoie_message(client_socket_fd, data)){
      printf("[/!\\] An error occured while sending a message\n");
    }
  }
  else if(!strncmp(code, CODE_NAM, 3)){
    if(renvoie_name(client_socket_fd, data)){
      printf("[/!\\] An error occured while sending a message\n");
    }
  }
  else if(!strncmp(code, CODE_TAG, 3)){
    if(recois_balises(client_socket_fd, data)){
      printf("[/!\\] An error occured while sending a message\n");
    }
  }
  else if(!strncmp(code, CODE_COL, 3)){
    if(recois_couleurs(client_socket_fd, data)){
      printf("[/!\\] An error occured while sending a message\n");
    }
  }
  else if(!strncmp(code, CODE_CAL, 3)){
    if(calcul(client_socket_fd, data)){
      printf("[/!\\] An error occured while sending a messages\n");
    }
  }
  else if(strcmp(data, END_CONN) == 0){
    printf("[-] Client disconnected\n");
    close(client_socket_fd);
    exit(EXIT_SUCCESS);
  }
  else {
    save_in_file(data, COLORS_DATABASE);
    plot(data);
    memset(data, 0, sizeof(char)*1024);
    create_ok_message(data, "Colors saved");
    if(write(client_socket_fd, (void *)data, strlen(data)) < 0){
      printf("[/!\\] An error occured while sending a message"); 
    }
  }
  free(code);
  return EXIT_SUCCESS;
}

int recois_couleurs(int client_socket_fd, char *data){
  FILE *fptr;

  fptr = fopen(FILE_COLORS, "a");

  if (fptr == NULL){
    memset(data, 0, sizeof(char)*DATA_LEN);
    create_error_message(data, "colors couldn't be saved");
    if(write(client_socket_fd, (void*)data, strlen(data)) < 0){
      printf("[/!\\] An error occured while sending messages\n");
    }
    exit(EXIT_FAILURE);
  }

  fprintf(fptr, "%s", data);
  fclose(fptr);

  create_ok_message(data, "Colors saved");
  int data_size = write(client_socket_fd, (void *)data, strlen(data));

  if (data_size < 0){
    perror("[/!\\] error while writing file");
    return EXIT_FAILURE;
  }

  memset(data, 0, sizeof(char)*DATA_LEN);

  return EXIT_SUCCESS;
}

int recois_balises(int socketfd, char *data){
  if (save_in_file(data, TAGS_DATABASE)){
    perror("[/!\\] Error saving tags");  strcpy(data, FIRST_JSON_PART);
    create_error_message(data, "tags couldn't be saved");
    return EXIT_FAILURE;
  }

  create_ok_message(data, "tags saved");

  if(renvoie_message(socketfd, data)){
    perror("[/!\\] Error responding to client");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int save_in_file(char *tags, const char* file_to_save){

  FILE *fd = fopen(file_to_save, "a");
  
  if (fd == NULL){
    perror("[/!\\] Error opening the file");
    return EXIT_FAILURE;
  }

  if (fprintf(fd, "%s\n", tags) < 0){
    perror("[/!\\] Error writing tags");
    return EXIT_FAILURE;
  }

  fclose(fd);
  return EXIT_SUCCESS;
}