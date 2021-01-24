CC = gcc
CFLAGS = -Wall

CIBLES = client serveur

all: $(CIBLES)

clean:
	$(RM) -f core *.o $(CIBLES) *~

run_serveur: clean all
	./serveur

run_client: all
	./client