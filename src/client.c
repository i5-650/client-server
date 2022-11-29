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
#include <arpa/inet.h>

#include "common.h"
#include "client.h"
#include "json.h"
#include "validation.h"


int socketfd;

int main(int argc, char *argv[]){
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

  if(argc != 2 || !inet_aton(argv[1], &server_addr.sin_addr)){
    server_addr.sin_addr.s_addr = INADDR_ANY;
  }

  // demande de connection au serveur
  int connect_status = connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (connect_status < 0){
    perror("connection serveur");
    exit(EXIT_FAILURE);
  }

  envoie_recois_name(socketfd);

  while (1){
    command_builder(socketfd);
  }
  close(socketfd);
}

void manage_signal(int sig){
  if(sig == SIGINT){
    if(write(socketfd, END_CONN, strlen(END_CONN)) < 0){
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
  printf("[+] Votre calcul (max 1000 caracteres): ");
  fgets(message, sizeof(message), stdin);

  if(set_calcul(message, data)){
    return EXIT_FAILURE;
  }

  int write_status = write(socketfd, data, strlen(data));
  if(write_status < 0){
    perror("[/!\\] erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(char)*DATA_LEN);

  // lire les données de la socket
  if(read_validated(socketfd, data)){
    return EXIT_FAILURE;
  }

  printf("[+] Message recu: %s\n", data);

  return 0;
}

int envoie_recois_message(int socketfd){
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(char)*DATA_LEN);

  // Demandez à l'utilisateur d'entrer un message
  char message[1024];
  printf("[+] Votre message (max 1000 caracteres): ");
  fgets(message, sizeof(message), stdin);

  if(set_message(message, data)){
    return EXIT_FAILURE;
  }

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0){
    perror("[/!\\] erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(char)*DATA_LEN);

  // lire les données de la socket
  if(read_validated(socketfd, data)){
    return EXIT_FAILURE;
  }

  printf("[+] Message recu: %s\n", data);

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

  if(set_name(hostname, data)){
    return EXIT_FAILURE;
  }

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0){
    perror("[/!\\] erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(char)*DATA_LEN);

  // lire les données de la socket
  if(read_validated(socketfd, data)){
    return EXIT_FAILURE;
  }

  printf("[+] Message recu: %s\n", data);

  return 0;
}

void analyse(char *pathname, char *data){
  // compte de couleurs
  couleur_compteur *cc = analyse_bmp_image(pathname);

  int count;
  char temp_string[10];
  prepare_message(data, CODE_ANL);
  add_first_element(data, "10");
  if (cc->size < 10){
    sprintf(data, "%d,", cc->size);
  }

  // choisir 10 couleurs
  for (count = 1; count < 11 && cc->size - count > 0; count++){
    if (cc->compte_bit == BITS32){
      sprintf(temp_string, "#%02x%02x%02x", cc->cc.cc24[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
    }

    if (cc->compte_bit == BITS24){
      sprintf(temp_string, "#%02x%02x%02x", cc->cc.cc32[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
    }

    add_element(data, temp_string);
  }
  remove_last_newline(data);
  //data[strlen(data) - 1] = '\0';
  strcat(data, "]}");
}

int envoie_couleurs(int socketfd){
  char data[1024];
  char pathname[1024];

  printf("\n[+] Veuillez renseigner le chemin d'accès de votre image:\n");
  fgets(pathname,sizeof(char)*DATA_LEN,stdin);
  pathname[strlen(pathname)-1] = '\0';

  memset(data, 0, sizeof(char)*DATA_LEN);
  analyse(pathname, data);

  int status = write(socketfd, data, strlen(data));
  if (status < 0){
    perror("[/!\\] Error writing to socket");
    exit(EXIT_FAILURE);
  }
  memset(data, 0, sizeof(char)*DATA_LEN);

  if(read_validated(socketfd, data)){
    return EXIT_FAILURE;
  }

  printf("[+] Message recu: %s\n", data);
  return 0;
}

int envoie_balise(int socketfd){
  char data[1024];
  char input[30];
  int balise_count = 0;
  
  printf("[+] How many tags are you sending ? (limited to 30) ");
  fgets(input, sizeof(char)*30, stdin);
  remove_last_newline(input);

  prepare_message(data, CODE_TAG);

  if(sscanf(input, "%d", &balise_count)<1){
    printf("[/!\\] Invalid number of tags\n");
    return EXIT_FAILURE;
  }
  
  if(balise_count <= 0 || balise_count > 30){
    printf("[/!\\] Invalid number of tags\n");
    return EXIT_FAILURE;
  }

  add_first_element(data, input);
  memset(input, 0, sizeof(input));

  for(int i = 0; i < balise_count; i++){
    printf("[+] Enter a tag (max len: 30): ");
    fgets(input, sizeof(char)*30, stdin);
    remove_last_newline(input);
    add_element(data, input);
    memset(input, 0, sizeof(char)*30);
  }

  strcat(data, "]}");

  if(write(socketfd, (void *)data, strlen(data)) < 0){
    perror("[/!\\] Error sending message");
    return EXIT_FAILURE;
  }

  memset(data, 0, sizeof(char)*DATA_LEN);

  if(read_validated(socketfd, data)){
    return EXIT_FAILURE;
  }

  if(strncmp(&data[strlen(FIRST_JSON_PART)], CODE_OKK, 3)){
    perror("[/!\\] An error occured on the server");
    return EXIT_FAILURE;
  }

  printf("[+] Messages received and saved\n");
  return EXIT_SUCCESS;
}

int envoie_num_min(int socketfd){
  char data[1024];
  char input[30];
  int number_count = 0;
  
  //Récupération du nombre de nombres
  printf("[+] How many nomber are you sending ? (limited to 30) ");
  fgets(input, sizeof(char)*30, stdin);
  remove_last_newline(input);

  prepare_message(data, CODE_MIN);

  if(sscanf(input, "%d", &number_count)<1){
    printf("[/!\\] Invalid size of list\n");
    return EXIT_FAILURE;
  }
  
  if(number_count <= 0 || number_count > 30){
    printf("[/!\\] Invalid size of list\n");
    return EXIT_FAILURE;
  }

  add_first_element(data, input);
  memset(input, 0, sizeof(input));

  for(int i = 0; i < number_count; i++){
    printf("[+] Enter a number : ");
    fgets(input, sizeof(char)*30, stdin);
    remove_last_newline(input);
    add_element(data, input);
    memset(input, 0, sizeof(char)*30);
  }

  strcat(data, "]}");

  if(write(socketfd, (void *)data, strlen(data)) < 0){
    perror("[/!\\] Error sending message");
    return EXIT_FAILURE;
  }

  memset(data, 0, sizeof(char)*DATA_LEN);

  if(read(socketfd, data, sizeof(char)*DATA_LEN) < 0){
    perror("[/!\\] Error receiving message");
    return EXIT_FAILURE;
  }

  if(strncmp(&data[strlen(FIRST_JSON_PART)], CODE_OKK, 3)){
    perror("[/!\\] An error occured on the server");
    return EXIT_FAILURE;
  }

  printf("[+] Messages received %s\n",data);

  printf("[+] Messages received and saved\n");
  return EXIT_SUCCESS;
}

int envoie_num_max(int socketfd){
  char data[1024];
  char input[30];
  int number_count = 0;
  
  //Récupération du nombre de nombres
  printf("[+] How many nomber are you sending ? (limited to 30) ");
  fgets(input, sizeof(char)*30, stdin);
  remove_last_newline(input);

  prepare_message(data, CODE_MAX);

  if(sscanf(input, "%d", &number_count)<1){
    printf("[/!\\] Invalid size of list\n");
    return EXIT_FAILURE;
  }
  
  if(number_count <= 0 || number_count > 30){
    printf("[/!\\] Invalid size of list\n");
    return EXIT_FAILURE;
  }

  add_first_element(data, input);
  memset(input, 0, sizeof(input));

  for(int i = 0; i < number_count; i++){
    printf("[+] Enter a number : ");
    fgets(input, sizeof(char)*30, stdin);
    remove_last_newline(input);
    add_element(data, input);
    memset(input, 0, sizeof(char)*30);
  }

  strcat(data, "]}");

  if(write(socketfd, (void *)data, strlen(data)) < 0){
    perror("[/!\\] Error sending message");
    return EXIT_FAILURE;
  }

  memset(data, 0, sizeof(char)*DATA_LEN);

  if(read(socketfd, data, sizeof(char)*DATA_LEN) < 0){
    perror("[/!\\] Error receiving message");
    return EXIT_FAILURE;
  }

  if(strncmp(&data[strlen(FIRST_JSON_PART)], CODE_OKK, 3)){
    perror("[/!\\] An error occured on the server");
    return EXIT_FAILURE;
  }

  printf("[+] Messages received %s\n",data);

  printf("[+] Messages received and saved\n");
  return EXIT_SUCCESS;
}

int envoie_couleurs_table(int socketfd){
 char data[1024];
  char input[30];
  int balise_count = 0;
  
  printf("[+] How many colors are you sending ? (limited to 30) ");
  fgets(input, sizeof(char)*30, stdin);
  remove_last_newline(input);

  prepare_message(data, CODE_TAG);

  if(sscanf(input, "%d", &balise_count)<1){
    printf("[/!\\] Invalid number of colors\n");
    return EXIT_FAILURE;
  }
  
  if(balise_count <= 0 || balise_count > 30){
    printf("[/!\\] Invalid number of colors\n");
    return EXIT_FAILURE;
  }

  add_first_element(data, input);
  memset(input, 0, sizeof(input));

  for(int i = 0; i < balise_count; i++){
    printf("[+] Enter a color (max len: 30): ");
    fgets(input, sizeof(char)*30, stdin);
    remove_last_newline(input);
    add_element(data, input);
    memset(input, 0, sizeof(char)*30);
  }

  strcat(data, "]}");

  if(write(socketfd, (void *)data, strlen(data)) < 0){
    perror("[/!\\] Error sending message");
    return EXIT_FAILURE;
  }

  memset(data, 0, sizeof(char)*DATA_LEN);

  if(read_validated(socketfd, data)){
    return EXIT_FAILURE;
  }

  if(strncmp(&data[strlen(FIRST_JSON_PART)], CODE_OKK, 3)){
    perror("[/!\\] An error occured on the server");
    return EXIT_FAILURE;
  }

  printf("[+] Messages received and saved\n");
  return EXIT_SUCCESS;
}


int command_builder(int socketfd){
  char command[10];

  printf("Veuiller choisir une fonction à exécuter (HELP pour plus d'informations !):");
  fgets(command, sizeof(char)*10, stdin);

  if (strcmp(command, "HELP\n") == 0){
    printf("[*] Voici la liste de toutes les commandes: \n\t[*] MSG: Permet d'envoyer un message au serveur avec une réponse de sa part !\n\t[*] CALC : Permet d'envoyer un calcul au serveur avec le résultat en retour !\n\t[*] COLOR : Permet d'envoyer des couleurs au serveur et de les sauvegarder dans un fichier !\n\t[*] TAGS : Permet d'envoyer dse balises au serveurs et les sauvegarder !\n\t[*] ANLZ : Permet d'analyser les couleurs d'une image et de les envoyer au serveur pour les sauvegarder.\n");
    return 0;
  }
  else if (strcmp(command, "MSG\n") == 0){
    printf("[+] Mode message activé : \n");
    envoie_recois_message(socketfd);
    return 0;
  }
  else if (strcmp(command, "CALC\n") == 0){
    printf("[+] Mode calcul activé : \n");
    if(envoie_recois_calcul(socketfd)){
      printf("TOUT CASSÉ\n");
    }
    return 0;
  }
  else if (strcmp(command, "COLOR\n") == 0){
    printf("[+] Mode couleurs activé : ");
    envoie_couleurs_table(socketfd);
    return 0;
  }
  else if(strcmp(command, "TAGS\n") == 0){
    printf("[+] Mode balises activé: \n");
    if(envoie_balise(socketfd)){
      printf("[/!\\] Error occured during tags sending\n");
      return EXIT_FAILURE;
    }
    return 0;
  }
  else if(strcmp(command, "ANLZ\n") == 0){
    printf("[+] Mode analise activé : \n");
    if(envoie_couleurs(socketfd)){
      printf("[/!\\] Error occured during image handling\n");
    }
  }
  else if(strcmp(command, "MIN\n") == 0){
    printf("[+] Mode minimum activé : \n");
    if(envoie_num_min(socketfd)){
      printf("[/!\\] Error occured during minimum sending\n");
    }
  }
  else if(strcmp(command, "MAX\n") == 0){
    printf("[+] Mode minimum activé : \n");
    if(envoie_num_max(socketfd)){
      printf("[/!\\] Error occured during minimum sending\n");
    }
  }

  return 1;
}

int read_validated(int socketfd, char *data){
  if(read(socketfd, data, sizeof(char)*DATA_LEN) < 0){
    perror("[/!\\] Error receiving message");
    return EXIT_FAILURE;
  }

  if(validate_json(data)){
    printf("[/!\\] Invalid JSON received\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}