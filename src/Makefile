SOURCES=

DEPS=\
../deps/ae/libae.a \
../deps/art/libart.a \
../deps/linenoise/liblinenoise.a \

DEPS_LDFLAGS= 
DEPS_LDFLAGS+= $(foreach dep,$(DEPS),-L$(dir $(dep)) -l$(subst lib,,$(notdir $(basename $(dep)))))

CFLAGS   = -g -std=c99 -Wpointer-arith -I../deps/ -D_DEFAULT_SOURCE 
LDFLAGS  = -lm -lpthread -lrt $(DEPS_LDFLAGS)

OBJECTS=$(SOURCES:%.c=%.o)

all: cli

cli: cli.c $(OBJECTS) $(DEPS)
	$(CC) $(CFLAGS) -o$@ $< $(OBJECTS) $(LDFLAGS)

%.a: 
	$(MAKE) -C $(@D)

.PHONY: clean

clean:
	$(RM) *.a *.o

.PHONY: indent
indent:
	find -name '*.c' | xargs clang-format -style=LLVM -i 
	find -name '*.h' | xargs clang-format -style=LLVM -i

scan:
	scan-build $(MAKE) clean all

