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

#ifndef __SERVER_H__
#define __SERVER_H__
#define FILE_COLORS "colors.txt"

#define PORT 8089

#define NOT_MY_GOAL -1337
#define TAGS_DATABASE "tags_database.txt"
#define COLORS_DATABASE "colors_database.txt"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "operations.h"
#include "common.h"

/* accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */


struct Client {
    int socketfd;
    char name[NAME_LEN];
};

int recois_envoie_message(struct Client* cli);
int recois_balises(char* data);
int save_in_file(char* tags, const char* file_to_save);
void plot(char *data);
int renvoie_message(char *data);
int recois_couleurs(char *data);
void* manage_client(void* client);
int check_validity(char *data);

#define MAX_CLIENTS 9
#define EXIT_END -1337

#endif
