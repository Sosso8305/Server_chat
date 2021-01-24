CC = gcc
CFLAGS = -Wall

CIBLES = client serveur

all: $(CIBLES)

clean:
	$(RM) -f core *.o $(CIBLES) *~

run_serveur:
	./serveur

run_client:
	./client