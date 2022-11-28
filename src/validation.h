
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* 
 * Le but principal de ce code est de s'assurer que les messages envoyés par le client sont
 * valides, c'est-à-dire que les messages respectent le format JSON et
 * respectent le protocole spécifié par le côté serveur.
 */

#ifndef __VALIDATION_H__
#define __VALIDATION_H__

#define INVALID_JSON_MSG "{\"code\":\"err\",\"values\":[\"Invalid JSON\"]}"


int validate_json(char* data);

#endif // __VALIDATION_H__