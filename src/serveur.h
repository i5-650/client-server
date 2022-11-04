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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "operations.h"
#include "common.h"

/* accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(int socketfd);
int recois_balises(int socketfd, char* data);
int save_tags(char* tags, int start_index);
void plot(char *data);
int renvoie_message(int client_socket_fd, char *data);
int recois_couleurs(int client_socket_fd, char *data);

#endif
