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

#ifndef __CLIENT_H__
#define __CLIENT_H__
#include "bmp.h"
#include "common.h"

/*
 * port d'ordinateur pour envoyer et recevoir des messages
 */
#define PORT 8089
#define NOT_MY_GOAL -1337

/*
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */
void manage_signal(int sig);
int envoie_recois_message(char* data);
int envoie_balise(char* data);
int command_builder(int socketfd, char* data);
int envoie_recois_calcul(char* data);
int envoie_recois_name(char* data);
void analyse(char *pathname, char *data);
int envoie_couleurs(char* data);
int envoie_couleurs_table(char* data);
int read_validated(int socketfd, char *data);

#endif
