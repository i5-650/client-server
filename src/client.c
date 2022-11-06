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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"
#include "client.h"

int main(int argc, char **argv){
  int socketfd;

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

/*
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */
int envoie_recois_calcul(int socketfd){
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // Demandez à l'utilisateur d'entrer un message
  char message[1024];
  printf("Votre calcul (max 1000 caracteres): ");
  fgets(message, sizeof(message), stdin);
  strcpy(data, HEADER_CALCUL);
  strcat(data, message);

  int write_status = write(socketfd, data, strlen(data));
  if(write_status < 0){
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
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
  memset(data, 0, sizeof(data));

  // Demandez à l'utilisateur d'entrer un message
  char message[1024];
  printf("Votre message (max 1000 caracteres): ");
  fgets(message, sizeof(message), stdin);
  strcpy(data, HEADER_MESSAGE);
  strcat(data, message);

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0){
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
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
  memset(data, 0, sizeof(data));

  // Demandez à l'utilisateur d'entrer un message
  char hostname[1024];
  hostname[1023] = '\0';
  gethostname(hostname, 1023);

  strcpy(data, HEADER_NAME);
  strcat(data, hostname);

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0){
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
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
  strcpy(data, HEADER_COLOR);
  char temp_string[10] = "10,";
  if (cc->size < 10){
    sprintf(temp_string, "%d,", cc->size);
  }
  strcat(data, temp_string);

  // choisir 10 couleurs
  for (count = 1; count < 11 && cc->size - count > 0; count++){
    if (cc->compte_bit == BITS32){
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc24[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
    }

    if (cc->compte_bit == BITS24){
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc32[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
    }

    strcat(data, temp_string);
  }

  // enlever le dernier virgule
  data[strlen(data) - 1] = '\0';
}

int envoie_couleurs(int socketfd, char *pathname){
  char data[1024];
  memset(data, 0, sizeof(data));
  analyse(pathname, data);

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0){
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  return 0;
}

int envoie_balise(int socketfd){
  char data[1024];
  char balises[1024];

  fgets(balises, sizeof(data), stdin);
  strcpy(data, HEADER_TAGS);
  strcat(data, balises);

  if(write(socketfd, (void *)data, strlen(data)) < 0){
    perror("Error sending message");
    return EXIT_FAILURE;
  }

  memset(data, 0, sizeof(data));

  if(read(socketfd, data, sizeof(data)) < 0){
    perror("Error receiving message");
    return EXIT_FAILURE;
  }

  if(!strcmp(data, "good")){
    perror("An error occured on the server");
    return EXIT_FAILURE;
  }

  printf("Messages received and saved\n");
  return EXIT_SUCCESS;
}

int envoie_couleurs_table(int socketfd){
  char color[1024];
  char data[1024];
  fgets(color, sizeof(color), stdin);
  strcpy(data, HEADER_COLOR);
  strcat(data, color);
  printf("%s", data);

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0){
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  return 0;
}

int command_builder(int socketfd){
  char command[10];

  printf("Veuiller choisir une fonction à exécuter (HELP pour plus d'informations !):");
  fgets(command, sizeof(command), stdin);

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
    envoie_recois_calcul(socketfd);
    return 0;
  }
  else if (strcmp(command, "COLOR\n") == 0){
    printf("Mode couleurs activé : ");
    envoie_couleurs_table(socketfd);
    return 0;
  }
  else if(strcmp(command, "TAGS\n") == 0){
    printf("Envoie de balise : ");
    envoie_balise(socketfd);
    return 0;
  }

  return 1;
}