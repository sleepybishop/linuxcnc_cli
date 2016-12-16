SOURCES=\
art.c \

CFLAGS   = -g -std=c99 -Wpointer-arith 
LDFLAGS  = 

OBJECTS=$(SOURCES:%.c=%.o)

all: libart.a

libart.a: $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)

clean:
	$(RM) *.o *.a


