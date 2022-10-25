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

void plot(char *data)
{

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
  while (1)
  {
    char *token = strtok_r(str, ",", &saveptr);
    if (token == NULL)
    {
      break;
    }
    str = NULL;
    printf("%d: %s\n", count, token);
    if (count == 1)
    {
      n = atoi(token);
      printf("n = %d\n", n);
    }
    else
    {
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
int renvoie_message(int client_socket_fd, char *data)
{
  int data_size = write(client_socket_fd, (void *)data, strlen(data));

  if (data_size < 0)
  {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

/* accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(int client_socket_fd)
{
    char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lecture de données envoyées par un client
  int data_size = read(client_socket_fd, (void *)data, sizeof(data));

  if (data_size < 0)
  {
    perror("erreur lecture");
    return (EXIT_FAILURE);
  }

  /*
   * extraire le code des données envoyées par le client.
   * Les données envoyées par le client peuvent commencer par le mot "message :" ou un autre mot.
   */
  printf("Données recus: %s\n", data);
  char code[10];
  sscanf(data, "%s", code);
  //memset(data, 0, sizeof(data));
  

  // Si le message commence par le mot: 'message:'
  if (strcmp(code, "message:") == 0) {
      renvoie_message(client_socket_fd, data);
  } else if(strcmp(code , "name:") == 0) {
      renvoie_message(client_socket_fd, data);
  } else if(strcmp(code, "balises: ")){
      recois_balises(client_socket_fd, data);
  } else if(strcmp(code, "couleurs:") == 0) {
      recois_couleurs(client_socket_fd,data);
  } else {
      //plot(data);
    calcul(client_socket_fd, data);
  }

  // fermer le socket
  //close(socketfd);
  return (EXIT_SUCCESS);
}

int recois_balises(int socketfd, char* data){
  char buff[9] = "empty";
  if(sscanf(buff, "%s", data)< 1){
    perror("Invalid input");
    return EXIT_FAILURE;
  }

  if(strcmp(buff, "balises: ") < 0){
    perror("Wrong usage !");
    return NOT_MY_GOAL;
  }

  int iterations = 0;
  if(sscanf(&data[9], "%d", &iterations) < 1 || iterations > 30){
    perror("Invalid number of balises");
    return EXIT_FAILURE;
  }

  unsigned short start_index = 9 + 1;
  if(iterations > 10){
    start_index++;
  }

  if(save_tags(data, start_index)){
    perror("Error saving tags");
    return EXIT_FAILURE;
  }
  if(renvoie_message(socketfd, "good")){
    perror("Error responding to client");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int save_tags(char* tags, int start_index){
  FILE* fd = fopen(TAGS_DATABASE, "a");
  if(fd == NULL){
    perror("Error opening the file");
    return EXIT_FAILURE;
  }
  if(fprintf(fd, "%s\n", &tags[start_index + 1]) < 0){
    perror("Error writing tags");
    return EXIT_FAILURE;
  }

  fclose(fd);
  return EXIT_SUCCESS;
=======
int calcul(int client_socket_fd, char* data){
  float result = calculator(data);
  if(sprintf(data, "%f", result) == EOF){
    perror("error parsing data");
    return (EXIT_FAILURE);
  }

  if(write(client_socket_fd, (void*)data, strlen(data)) < 0){
    perror("error sending datas");
    return EXIT_FAILURE;
  }

  return (EXIT_SUCCESS);

}

float calculator(char* data){
    if(data[0] != '+' && data[0] != '-' && data[0] != '*' && data[0] != '%' && data[0] != '/'){
        return 0.0;
    }
    float result = 0.0;
    struct Calc c;
    c.operator = data[0];
    if(sscanf(&data[1], "%f %f", &c.nums[0], &c.nums[1]) < 2){
      perror("Wrong input");
      return EXIT_FAILURE;
    }

    if(c.operator == '+'){
      result = c.nums[0] + c.nums[1];
    }
    else if(c.operator == '-'){
      result = c.nums[0] - c.nums[1];
    }
    else if(c.operator == '*'){
      result = c.nums[0] * c.nums[1];
    }
    else if(c.operator == '/'){
      result = c.nums[0] / c.nums[1];
    }
    else {
      return 0.0;
    }
    return result;

int recois_couleurs(int client_socket_fd,char* data)
{
    FILE *fptr;

    fptr = fopen(FILE_COLORS,"a");

    if(fptr == NULL)
    {
        printf("Error");
        exit(1);
    }

    fprintf(fptr,"%s",data);
    fclose(fptr);

    int data_size = write(client_socket_fd, (void *)data, strlen(data));

    if (data_size < 0)
    {
        perror("erreur ecriture");
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);

}

int main()
{

  int socketfd;
  int bind_status;

  struct sockaddr_in server_addr;

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0)
  {
    perror("Unable to open a socket");
    return -1;
  }

  int option = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  // détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Relier l'adresse à la socket
  bind_status = bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (bind_status < 0)
  {
    perror("bind");
    return (EXIT_FAILURE);
  }



  // Écouter les messages envoyés par le client
    listen(socketfd, 10);
    struct sockaddr_in client_addr;

    unsigned int client_addr_len = sizeof(client_addr);

    // nouvelle connection de client
    int client_socket_fd = accept(socketfd, (struct sockaddr *)&client_addr, &client_addr_len);

  // Lire et répondre au client
  while(1){
      recois_envoie_message(client_socket_fd);
  }


  return 0;
}
