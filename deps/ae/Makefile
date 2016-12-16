SOURCES=\
./ae.c \
./anet.c \

HEADERS=\
ae.h \
anet.h \
config.h \
fmacros.h \
zmalloc.h \

CFLAGS   = -g -std=c99
LDFLAGS  = 

OBJECTS=$(SOURCES:%.c=%.o)

all: libae.a

$(OBJECTS): $(HEADERS)

libae.a: $(OBJECTS) $(HEADERS)
	$(AR) rcs $@ $(OBJECTS)

.PHONY: clean
clean:
	$(RM) *.o *.a


