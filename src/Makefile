CC ?= gcc

# Modifiez les deux lignes suivantes et ajoutez de nouveaux fichiers selon vos besoins
CLIENT_OBJS ?= client.o bmp.o couleur.o json.o validation.o
SERVER_OBJS ?= serveur.o operations.o json.o validation.o

# Avertissements
CFLAGS ?= -Wall -Wextra -pthread

# Déboggage
DEBUG ?= -ggdb3

.SUFFIXES: .c .o

SERVER = serveur
CLIENT = client

all: $(SERVER) $(CLIENT)

# Création d'un exécutable pour le côté serveur
$(SERVER): $(SERVER_OBJS)
	$(CC) $(DEBUG) $(CFLAGS) -o $(SERVER).out $(SERVER_OBJS)

# Création d'un exécutable pour le côté client
$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(DEBUG) $(CFLAGS) -o $(CLIENT).out $(CLIENT_OBJS)

# Compilation du code sans création d'un exécutable
.c.o: 
	$(CC) $(DEBUG) $(CFLAGS) -c $*.c

clean: 
	rm -rf *.o *.out *.txt

