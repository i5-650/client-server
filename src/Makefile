CC ?= gcc

# Modifiez les deux lignes suivantes et ajoutez de nouveaux fichiers selon vos besoins
CLIENT_OBJS ?= client.o bmp.o couleur.o json.o validation.o
SERVER_OBJS ?= serveur.o operations.o json.o validation.o
TESTES_OBJS ?= testes.o operations.o json.o validation.o

# Avertissements
CFLAGS ?= -Wall -Wextra -pthread -g

# Déboggage
DEBUG ?= -ggdb3

.SUFFIXES: .c .o

SERVER = serveur
CLIENT = client
TESTES = testes

all: $(SERVER) $(CLIENT) $(TESTES)

# Création d'un exécutable pour le côté serveur
$(SERVER): $(SERVER_OBJS)
	$(CC) $(DEBUG) $(CFLAGS) -o $(SERVER).out $(SERVER_OBJS) -lm

# Création d'un exécutable pour le côté client
$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(DEBUG) $(CFLAGS) -o $(CLIENT).out $(CLIENT_OBJS) -lm

$(TESTES): $(TESTES_OBJS)
	$(CC) $(DEBUG) $(CFLAGS) -o $(TESTES).out $(TESTES_OBJS) -lm

# Compilation du code sans création d'un exécutable
.c.o: 
	$(CC) $(DEBUG) $(CFLAGS) -c $*.c

clean: 
	rm -rf *.o *.out *.txt

tests:
	./testes.out