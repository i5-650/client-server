/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Code du côté client. Le but principal de ce code est de communiquer avec le serveur,
 * d'envoyer et de recevoir des messages. Ces messages peuvent être du simple texte ou des chiffres.
 */

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#include "common.h"
#include "client.h"
#include "json.h"


int socketfd;

int main(int argc, char **argv){
  signal(SIGINT, manage_signal);

  struct sockaddr_in server_addr;

  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0){
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // demande de connection au serveur
  int connect_status = connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (connect_status < 0){
    perror("connection serveur");
    exit(EXIT_FAILURE);
  }

  if (argc != 2){
    envoie_recois_name(socketfd);
    while (1){
      command_builder(socketfd);
    }
  }
  else{
    envoie_couleurs(socketfd, argv[1]);
  }

  close(socketfd);
}

void manage_signal(int sig){
  if(sig == SIGINT){
    if(write(socketfd, "exit", 4) < 0){
      perror("write");
      exit(EXIT_FAILURE);
    }
    close(socketfd);
    exit(EXIT_SUCCESS);
  }
}

int envoie_recois_calcul(int socketfd){
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(char)*DATA_LEN);

  // Demandez à l'utilisateur d'entrer un message
  char message[1024];
  printf("Votre calcul (max 1000 caracteres): ");
  fgets(message, sizeof(message), stdin);

  strcpy(data, FIRST_JSON_PART);
  strcat(data, CODE_CAL);
  strcat(data, ARRAY_JSON_PART);
  
  if(set_calcul(message, data)){
    return EXIT_FAILURE;
  }

  int write_status = write(socketfd, data, strlen(data));
  if(write_status < 0){
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(char)*DATA_LEN);

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(char)*DATA_LEN);
  if(read_status < 0){
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);

  return 0;
}

int envoie_recois_message(int socketfd){
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(char)*DATA_LEN);

  // Demandez à l'utilisateur d'entrer un message
  char message[1024];
  printf("Votre message (max 1000 caracteres): ");
  fgets(message, sizeof(message), stdin);


  strcpy(data, FIRST_JSON_PART);
  strcat(data, CODE_MSG);
  strcat(data, ARRAY_JSON_PART);
  if(set_message(message, data)){
    return EXIT_FAILURE;
  }

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0){
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(char)*DATA_LEN);

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(char)*DATA_LEN);
  if (read_status < 0){
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);

  return 0;
}

int envoie_recois_name(int socketfd){
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(char)*DATA_LEN);

  // Demandez à l'utilisateur d'entrer un message
  char hostname[1024];
  hostname[1023] = '\0';
  gethostname(hostname, 1023);

  strcpy(data, FIRST_JSON_PART);
  strcat(data, CODE_NAM);
  strcat(data, ARRAY_JSON_PART);

  if(set_message(hostname, data)){
    return EXIT_FAILURE;
  }

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0){
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(char)*DATA_LEN);

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(char)*DATA_LEN);
  if (read_status < 0){
    perror("erreur lecture");
    return -1;
  }

  printf("Nom recu: %s\n", data);

  return 0;
}

void analyse(char *pathname, char *data){
  // compte de couleurs
  couleur_compteur *cc = analyse_bmp_image(pathname);

  int count;
  char temp_string[1024] = FIRST_JSON_PART;
  strcat(temp_string, CODE_ANL);
  strcat(temp_string, ARRAY_JSON_PART);
  strcat(temp_string,"\"10\",");
  if (cc->size < 10){
    sprintf(temp_string, "%d,", cc->size);
  }
  strcat(data, temp_string);

  // choisir 10 couleurs
  for (count = 1; count < 11 && cc->size - count > 0; count++){
    if (cc->compte_bit == BITS32){
      sprintf(temp_string, "\"#%02x%02x%02x\",", cc->cc.cc24[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
    }

    if (cc->compte_bit == BITS24){
      sprintf(temp_string, "\"#%02x%02x%02x\",", cc->cc.cc32[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
    }

    strcat(data, temp_string);
  }
  data[strlen(data) - 1] = '\0';
  strcat(data, "]}");
}

int envoie_couleurs(int socketfd, char* image_path){
  char data[1024];
  char pathname[1024];
  
  if(image_path == NULL){
    printf("Veuillez entrer le chemin de l'image: ");
    fgets(image_path, sizeof(char)*DATA_LEN, stdin);
    image_path[strlen(image_path) - 1] = '\0';
  }
  else {
    strncpy(data, image_path, 1024);
  }


  printf("\nVeuillez renseigner le chemin d'accès de votre image:\n");
  fgets(pathname,sizeof(char)*DATA_LEN,stdin);
  pathname[strlen(pathname)-1] = '\0';

  memset(data, 0, sizeof(char)*DATA_LEN);
  analyse(pathname, data);
  printf("\ndata: %s", data);

  int status = write(socketfd, data, strlen(data));
  if (status < 0){
    perror("Error writing to socket");
    exit(EXIT_FAILURE);
  }

  status = read(socketfd, data, sizeof(char)*DATA_LEN);
  if(status < 0){
    perror("Error receiving server response");
    return EXIT_FAILURE;
  }

  if(strncmp(&data[strlen(ARRAY_JSON_PART)], CODE_OKK, 3) == 0){
    printf("Colors saved\n");
    return EXIT_SUCCESS;
  }
  return 0;
}

int envoie_balise(int socketfd){
  char data[1024];
  char input[30];
  int balise_count = 0;
  
  printf("How many tags are you sending ? (limited to 30) ");
  fgets(input, sizeof(char)*30, stdin);
  if(input[strlen(input) - 1] == '\n'){
    input[strlen(input) - 1] = '\0';
  }

  strcpy(data, FIRST_JSON_PART);
  strcat(data, CODE_TAG);
  strcat(data, ARRAY_JSON_PART);
  strcat(data, "\"");

  sscanf(input, "%d", &balise_count);
  
  if(balise_count <= 0 || balise_count > 30){
    return EXIT_FAILURE;
  }
  
  strcat(data, input);
  strcat(data, "\",\"");
  memset(input, 0, sizeof(input));

  for(int i = 0; i < balise_count; i++){
    printf("Enter a tag (max len: 30): ");
    fgets(input, sizeof(char)*30, stdin);
    if(input[strlen(input) - 1] == '\n'){
      input[strlen(input) - 1] = '\0';
    }
    strcat(data, input);
    strcat(data, "\"");
    if(i + 1 != balise_count){
      strcat(data, ",\"");
    }
    memset(input, 0, sizeof(char)*30);
  }

  strcat(data, "]}");

  if(write(socketfd, (void *)data, strlen(data)) < 0){
    perror("Error sending message");
    return EXIT_FAILURE;
  }

  memset(data, 0, sizeof(char)*DATA_LEN);

  if(read(socketfd, data, sizeof(char)*DATA_LEN) < 0){
    perror("Error receiving message");
    return EXIT_FAILURE;
  }

  if(strncmp(&data[strlen(FIRST_JSON_PART)], CODE_OKK, 3)){
    perror("An error occured on the server");
    return EXIT_FAILURE;
  }

  printf("Messages received and saved\n");
  return EXIT_SUCCESS;
}

int envoie_couleurs_table(int socketfd){
  char data[1024];
  char input[30];
  int balise_count = 0;
  
  printf("How many colors are you sending ? (limited to 30) ");
  fgets(input, sizeof(char)*30, stdin);
  if(input[strlen(input) - 1] == '\n'){
    input[strlen(input) - 1] = '\0';
  }

  strcpy(data, FIRST_JSON_PART);
  strcat(data, CODE_TAG);
  strcat(data, ARRAY_JSON_PART);
  strcat(data, "\"");

  sscanf(input, "%d", &balise_count);
  
  if(balise_count <= 0 || balise_count > 30){
    return EXIT_FAILURE;
  }
  
  strcat(data, input);
  strcat(data, "\",\"");
  memset(input, 0, sizeof(char)*30);

  for(int i = 0; i < balise_count; i++){
    printf("Enter a colors (max len: 30): ");
    fgets(input, sizeof(char)*30, stdin);
    if(input[strlen(input) - 1] == '\n'){
      input[strlen(input) - 1] = '\0';
    }
    strcat(data, input);
    strcat(data, "\"");
    if(i + 1 != balise_count){
      strcat(data, ",\"");
    }
    memset(input, 0, sizeof(char)*30);
  }

  strcat(data, "]}");

  if(write(socketfd, (void *)data, strlen(data)) < 0){
    perror("Error sending message");
    return EXIT_FAILURE;
  }

  memset(data, 0, sizeof(char)*DATA_LEN);

  if(read(socketfd, data, sizeof(char)*DATA_LEN) < 0){
    perror("Error receiving message");
    return EXIT_FAILURE;
  }

  if(strncmp(&data[strlen(FIRST_JSON_PART)], CODE_OKK, 3)){
    perror("An error occured on the server");
    return EXIT_FAILURE;
  }

  printf("Messages received and saved\n");
  return EXIT_SUCCESS;
  return 0;
}

int command_builder(int socketfd){
  char command[10];

  printf("Veuiller choisir une fonction à exécuter (HELP pour plus d'informations !):");
  fgets(command, sizeof(char)*10, stdin);

  if (strcmp(command, "HELP\n") == 0){
    printf("Voici la liste de toutes les commandes: \nMSG: Permet d'envoyer un message au serveur avec une réponse de sa part !\nCALC : Permet d'envoyer un calcul au serveur avec le résultat en retour !\nCOLOR : Permet d'envoyer des couleurs au serveur et de les sauvegarder dans un fichier !\nTAGS : Permet d'envoyer dse balises au serveurs et les sauvegarder !\n");
    return 0;
  }
  else if (strcmp(command, "MSG\n") == 0){
    printf("Mode message activé : \n");
    envoie_recois_message(socketfd);
    return 0;
  }
  else if (strcmp(command, "CALC\n") == 0){
    printf("Mode calcul activé : \n");
    if(envoie_recois_calcul(socketfd)){
      printf("TOUT CASSÉ\n");
    }
    return 0;
  }
  else if (strcmp(command, "COLOR\n") == 0){
    printf("Mode couleurs activé : ");
    envoie_couleurs_table(socketfd);
    return 0;
  }
  else if(strcmp(command, "TAGS\n") == 0){
    if(envoie_balise(socketfd)){
      printf("Error occured during tags sending\n");
      return EXIT_FAILURE;
    }
    return 0;
  }
  else if(strcmp(command, "ANLZ\n") == 0){
    printf("Mode analise activé : ");
    envoie_couleurs(socketfd, NULL);
  }

  return 1;
}