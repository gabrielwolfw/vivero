CC = mpicc
CFLAGS = -Wall
LIBS = -lcurl -ljson-c -lssl -lcrypto -lm

SOURCES = test2.c sistema_distribuido.c seguridad.c calculos.c prediccion_clima.c
EXECUTABLE = vivero_distribuido

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(LIBS)

clean:
	rm -f $(EXECUTABLE)

.PHONY: all clean