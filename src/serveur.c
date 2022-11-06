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
#include <stdio.h>
#include <string.h>

int main(){
  int socketfd;
  int bind_status;
  struct sockaddr_in server_addr;

  //Creation d'une socket
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0){
    perror("Unable to open a socket");
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
    perror("Error during first connection: ");
  }

  while (1){
    recois_envoie_message(client_socket_fd);
  }

  return 0;
}

void plot(char *data){
  // Extraire le compteur et les couleurs RGB
  FILE *p = popen("gnuplot -persist", "w");
  printf("Plot\n");
  int count = 0;
  int n;
  char *saveptr = NULL;
  char *str = data;

  fprintf(p, "set xrange [-15:15]\n");
  fprintf(p, "set yrange [-15:15]\n");
  fprintf(p, "set style fill transparent solid 0.9 noborder\n");
  fprintf(p, "set title 'Top 10 colors'\n");
  fprintf(p, "plot '-' with circles lc rgbcolor variable\n");

  while (1){
    char *token = strtok_r(str, ",", &saveptr);
    
    if (token == NULL){
      break;
    }

    str = NULL;
    printf("%d: %s\n", count, token);

    if (count == 1){
      n = atoi(token);
      printf("n = %d\n", n);
    }
    else{
      // Le numéro 36, parceque 360° (cercle) / 10 couleurs = 36
      fprintf(p, "0 0 10 %d %d 0x%s\n", (count - 1) * 36, count * 36, token + 1);
    }
    count++;
  }

  fprintf(p, "e\n");
  printf("Plot: FIN\n");
  pclose(p);
}

/* renvoyer un message (*data) au client (client_socket_fd)
 */
int renvoie_message(int client_socket_fd, char *data){
  int data_size = write(client_socket_fd, (void *)data, strlen(data));

  if (data_size < 0){
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }

  return (EXIT_SUCCESS);
}

/* accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(int client_socket_fd){
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lecture de données envoyées par un client
  int data_size = read(client_socket_fd, (void *)data, sizeof(data));

  if (data_size < 0){
    perror("erreur lecture");
    return EXIT_FAILURE;
  }

  /*
   * extraire le code des données envoyées par le client.
   * Les données envoyées par le client peuvent commencer par le mot "message :" ou un autre mot.
   */
  printf("Données recus: %s\n", data);
  char code[10];
  sscanf(data, "%s", code);

  if(!strncmp(code, HEADER_MESSAGE, strlen(HEADER_MESSAGE) - 1)){
    printf("Message case\n");
    renvoie_message(client_socket_fd, data);
  }
  else if(!strncmp(code, HEADER_NAME, strlen(HEADER_NAME) - 1)){
    printf("Name case\n");
    renvoie_message(client_socket_fd, data);
  }
  else if(!strncmp(code, HEADER_TAGS, strlen(HEADER_TAGS) - 1)){
    printf("Tags case\n");
    recois_balises(client_socket_fd, data);
  }
  else if(!strncmp(code, HEADER_COLOR, strlen(HEADER_COLOR) - 1)){
    printf("Color case\n");
    recois_couleurs(client_socket_fd, data);
  }
  else if(!strncmp(code, HEADER_CALCUL, strlen(HEADER_CALCUL) - 1)){
    printf("Calcul case\n");
    calcul(client_socket_fd, data);
  }
  else {
    printf("Couldn't satisfy command\n");
  }

  // fermer le socket
  // close(socketfd);
  return EXIT_SUCCESS;
}

int recois_couleurs(int client_socket_fd, char *data){
  FILE *fptr;

  fptr = fopen(FILE_COLORS, "a");

  if (fptr == NULL){
    printf("Error");
    exit(1);
  }

  fprintf(fptr, "%s", data);
  fclose(fptr);

  int data_size = write(client_socket_fd, (void *)data, strlen(data));

  if (data_size < 0){
    perror("erreur ecriture");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int recois_balises(int socketfd, char *data){
  unsigned int starting_index = strlen(HEADER_TAGS);
  int iterations = 0;
  if (sscanf(&data[starting_index], "%d", &iterations) < 1 || iterations > 30){
    perror("Invalid number of balises");
    return EXIT_FAILURE;
  }

  ++starting_index;
  if (iterations > 10){
    ++starting_index;
  }

  if (save_tags(data, starting_index)){
    perror("Error saving tags");
    return EXIT_FAILURE;
  }
  if (renvoie_message(socketfd, "good")){
    perror("Error responding to client");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int save_tags(char *tags, int start_index){
  FILE *fd = fopen(TAGS_DATABASE, "a");
  
  if (fd == NULL){
    perror("Error opening the file");
    return EXIT_FAILURE;
  }

  if (fprintf(fd, "%s\n", &tags[start_index + 1]) < 0){
    perror("Error writing tags");
    return EXIT_FAILURE;
  }

  fclose(fd);
  return EXIT_SUCCESS;
}