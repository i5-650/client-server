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

#define PORT 8089

struct Calc {
 char operator;
 float nums[2];
}Calc;
/* accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(int socketfd);
float calculator(char* data);
int calcul(int client_socket_fd, char* data);

#endif
