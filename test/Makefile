BIN=test
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

CC=cc
CFLAGS=
LFLAGS=-ljson -lcurl -O2 -g3

$(BIN): $(OBJS)
	$(CC) -o $(BIN) $(LFLAGS) $(OBJS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) $(BIN)

