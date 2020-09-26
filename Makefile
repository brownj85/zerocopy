CC=gcc
CFLAGS=-g -Wall -DZCOPYSOCKET_PYTHON
INCL=-I$(shell pwd) -lpython3.8 -I/usr/include/python3.8

BUILDIR=.

SRC=$(wildcard *.c)
HDR=$(wildcard *.h)

OBJ=$(patsubst %.c,$(BUILDIR)/%.o,$(SRC))

lib: $(BUILDIR)/lib$(SHARED_LIB_TARGET).so

$(BUILDIR)/%.o: %.c %.h
	$(CC) $(CFLAGS) -fPIC $(INCL) -c $< -o $@

$(BUILDIR)/%.o: %.c
	$(CC) $(CFLAGS) -fPIC $(INCL) -c $< -o $@

$(BUILDIR)/lib$(SHARED_LIB_TARGET).so: $(OBJ)
	echo $(OBJ)
	$(CC) -shared -o $@ $^

test: $(SRC)
	$(CC) $^ -o $@
