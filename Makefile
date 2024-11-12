CC = mpicc
CFLAGS = -Wall -Wextra
LIBS = -lcurl -ljson-c -lm

SRCS = main.c sistema_distribuido.c prediccion_clima.c calculos.c
OBJS = $(SRCS:.c=.o)
TARGET = vivero_distribuido

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)