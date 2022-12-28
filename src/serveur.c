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
#include <pthread.h>
#ifdef __APPLE__
  #include <dispatch/dispatch.h>
#else
  #include <semaphore.h>
#endif
#include "json.h"
#include "validation.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#ifdef __APPLE__
  dispatch_semaphore_t sem;
#else
  sem_t sem;
#endif

int clients_count = 0;

#ifndef TESTS
  int main(int argc, char *argv[]){
    int socketfd;
    int bind_status;
    struct sockaddr_in server_addr;
    unsigned short max_client = MAX_CLIENTS;
    if(argc == 2 && sscanf(argv[1], "%hu", &max_client) < 1){
      printf("Usage: ./serveur <max_clients>");
      return EXIT_FAILURE;
    }

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
    #ifdef __APPLE__
      sem = dispatch_semaphore_create(max_client);
    #else
      sem_init(&sem, 0, MAX_CLIENTS);
    #endif
    
    while(1){
        #ifdef __APPLE__
          dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
        #else
          sem_wait(&sem);
        #endif
        struct Client* client = malloc(sizeof(struct Client));
        client->socketfd = accept(socketfd, (struct sockaddr *)&client_addr, &client_addr_len);
        printf("[+] New client connected !\n");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, manage_client, (void *)client);
    }
    #ifdef __APPLE__
      dispatch_release(sem);
    #else
      sem_destroy(&sem);
    #endif

    return 0;
  }
#endif

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
      sscanf(token,"%d",&n);
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
int renvoie_message(char *data){
  // we only check if the message is at least 1 char long
  if(strlen(data) > strlen(FIRST_JSON_PART) + 3 + strlen(ARRAY_JSON_PART) + 3){
    int start = strlen(FIRST_JSON_PART);
    data[start] = CODE_OKK[0];
    data[++start] = CODE_OKK[1];
    data[++start] = CODE_OKK[2];
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}

int renvoie_name(char *data){
  int status = renvoie_message(data);
  return status;
}

/* accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(struct Client* cli){
  char data[1024];
  memset(data, 0, sizeof(char)*DATA_LEN);


  int data_size = read(cli->socketfd, (void *)data, sizeof(char)*DATA_LEN);
  if (data_size < 0){
    perror("erreur lecture");
    return EXIT_FAILURE;
  }

  if(!data_size){
    printf("[+] Client disappeared\n");
    return EXIT_END;
  }

  if(check_validity(data)){
    create_error_message(data, "couldn't parse data");
    if(write(cli->socketfd, (void *)data, strlen(data)) < 0){
      perror("[/!\\] Error sending message");
      return EXIT_FAILURE;
    }
  }

  printf("[%s] Données recus: %s\n",cli->name, data);
  char *code = get_code(data);

  if(!strncmp(code, CODE_MSG, 3)){
    if(renvoie_message(data)){
      printf("[/!\\] An error occured while sending a message\n");
    }
  }
  else if(!strncmp(code, CODE_NAM, 3)){
    strncpy(cli->name, &data[strlen(FIRST_JSON_PART) + 3 + strlen(ARRAY_JSON_PART) + 1], NAME_LEN);
    cli->name[strlen(cli->name) - 3] = '\0';
    if(renvoie_name(data)){
      printf("[/!\\] An error occured while sending a message\n");
    }
  }
  else if(!strncmp(code, CODE_TAG, 3)){
    if(recois_balises(data)){
      printf("[/!\\] An error occured while sending a message\n");
    }
  }
  else if(!strncmp(code, CODE_COL, 3)){
    if(recois_couleurs(data)){
      printf("[/!\\] An error occured while sending a message\n");
    }
  }
  else if(!strncmp(code, CODE_CAL, 3)){
    if(calcul(cli->socketfd, data)){
      printf("[/!\\] An error occured while sending a messages\n");
    }
  }
  else if(!strncmp(code, CODE_MIN, 3)){
    if(mini(cli->socketfd,data)){
      printf("[/!\\] An error occured while sending a messages\n");
    }
  }
  else if(!strncmp(code, CODE_MAX, 3)){
    if(maxi(cli->socketfd,data)){
      printf("[/!\\] An error occured while sending a messages\n");
    }
  }
  else if (!strncmp(code, CODE_AVG, 3)){
    if(avg(cli->socketfd, data)){
      printf("[/!\\] An error occured while sending a messages\n");
    }
  }
  else if (!strncmp(code, CODE_ECT, 3)){
    if(ecart(cli->socketfd, data)){
      printf("[/!\\] An error occured while sending a messages\n");
    }
  }
  else if(strcmp(data, END_CONN) == 0){
    printf("[-] Client disconnected\n");
    close(cli->socketfd);
    return EXIT_END;
  }
  else if(strcmp(data, CODE_ANL) == 0){
    save_in_file(data, COLORS_DATABASE);
    plot(data);
    memset(data, 0, sizeof(char)*1024);
    create_ok_message(data, "Colors saved");
  }
  else{
    create_error_message(data, "unknown code");
  }

  if(write(cli->socketfd, (void *)data, strlen(data)) < 0){
    printf("[/!\\] An error occured while sending a message"); 
  }

  return EXIT_SUCCESS;
}

int recois_couleurs(char *data){
  FILE *fptr;

  fptr = fopen(FILE_COLORS, "a");

  if (fptr == NULL){
    memset(data, 0, sizeof(char)*DATA_LEN);
    create_error_message(data, "colors couldn't be saved");
    return EXIT_FAILURE;
  }

  fprintf(fptr, "%s", data);
  fclose(fptr);

  create_ok_message(data, "Colors saved");
  return EXIT_SUCCESS;
}

int recois_balises(char *data){
  if(save_in_file(data, TAGS_DATABASE)){
    printf("[/!\\] Error saving tags");
    create_error_message(data, "tags couldn't be saved");
    return EXIT_FAILURE;
  }

  create_ok_message(data, "tags saved");
  return EXIT_SUCCESS;
}


int save_in_file(char *tags, const char* file_to_save){
  pthread_mutex_lock(&mutex);

  FILE *fd = fopen(file_to_save, "a");
  
  if(fd == NULL){
    perror("[/!\\] Error opening the file");
    return EXIT_FAILURE;
  }

  if(fprintf(fd, "%s\n", tags) < 0){
    perror("[/!\\] Error writing tags");
    return EXIT_FAILURE;
  }

  fclose(fd);
  pthread_mutex_unlock(&mutex);
  return EXIT_SUCCESS;
}

void* manage_client(void* client){
  int status = recois_envoie_message(client);
    if(status < 0){
      perror("[/!\\] Error during first connection: ");
    }

    while (status != EXIT_END){
      status = recois_envoie_message(client);
    }
    #ifdef __APPLE__
      dispatch_semaphore_signal(sem);
    #else
      sem_post(&sem);
    #endif
    free(client);
    pthread_exit(NULL);
    return NULL;
}

int check_validity(char *data){
  if(!json_validator(data)){
    printf("[/!\\] Invalid JSON received\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}