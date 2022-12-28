CC ?= gcc

# Modifiez les deux lignes suivantes et ajoutez de nouveaux fichiers selon vos besoins
CLIENT_OBJS ?= client.o bmp.o couleur.o json.o validation.o
SERVER_OBJS ?= serveur.o operations.o json.o validation.o
TESTES_OBJS ?= client.o bmp.o couleur.o json.o validation.o operations.o serveur.o testes.o

# Avertissements
CFLAGS ?= -Wall -Wextra -pthread -g -lm
RElEASE_FLAGS ?= -Wall -Wextra -pthread -Ofast -ffast-math -finline-functions -lm

# Déboggage
DEBUG ?= -ggdb3

.SUFFIXES: .c .o

SERVER = serveur
CLIENT = client
TESTES = testes
RLS_SERVER = serveur_release
RLS_CLIENT = client_release

all: $(SERVER) $(CLIENT)

# Création d'un exécutable pour le côté serveur
$(SERVER): $(SERVER_OBJS)
	$(CC) $(DEBUG) $(CFLAGS) -o $(SERVER).out $(SERVER_OBJS)

# Création d'un exécutable pour le côté client
$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(DEBUG) $(CFLAGS) -o $(CLIENT).out $(CLIENT_OBJS)


release: $(RLS_SERVER) $(RLS_CLIENT)

$(RLS_SERVER): $(SERVER_OBJS)
	$(CC) $(RElEASE_FLAGS) -o $(RLS_SERVER).out $(SERVER_OBJS)

$(RLS_CLIENT): $(CLIENT_OBJS)
	$(CC) $(RElEASE_FLAGS) -o $(RLS_CLIENT).out $(CLIENT_OBJS)


$(TESTES):
	$(CC) -DTESTS="true" *.c $(CFLAGS) -o $(TESTES).out 

# Compilation du code sans création d'un exécutable
.c.o: 
	$(CC) $(DEBUG) $(CFLAGS) -c $*.c

clean: 
	rm -rf *.o *.out *.txt

tests:
	./testes.out