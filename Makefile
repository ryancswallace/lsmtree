LIBS  = -lm
CFLAGS = -Wall

SRC=src/*.c

lsm: $(SRC)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)