SOURCES=\
./linenoise.c \

CFLAGS   = -g -std=c99 -Wpointer-arith -D_DEFAULT_SOURCE 
LDFLAGS  = 

OBJECTS=$(SOURCES:%.c=%.o)

all: liblinenoise.a

liblinenoise.a: $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)

.PHONY: clean
clean:
	$(RM) *.o *.a


