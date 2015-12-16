GCOV_CCFLAGS = -fprofile-arcs -ftest-coverage
GCOV_OUTPUT = *.gcda *.gcno *.gcov 
CC     = gcc
CFLAGS = -g -O2 -Wall -Werror -W -I. -fno-omit-frame-pointer -fno-common \
	  -fsigned-char -fPIC \
	  $(GCOV_CCFLAGS)

UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
SHAREDFLAGS = -dynamiclib
SHAREDEXT = dylib
else
SHAREDFLAGS = -shared
SHAREDEXT = so
endif

all: test_bencode static shared

OBJECTS = bencode.o

.PHONY: shared
shared: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(CFLAGS) -fPIC $(SHAREDFLAGS) -o libbencode.$(SHAREDEXT)

.PHONY: static
static: $(OBJECTS)
	ar -r libbencode.a $(OBJECTS)

main.c:
	sh tests/make-tests.sh tests/test_bencode.c > main.c

test_bencode: main.c bencode.o tests/test_bencode.c tests/CuTest.c
	$(CC) $(CFLAGS) -Itests -o $@ $^
	./test_bencode
	gcov main.c bencode.c

bencode_consumer: bencode_consumer.c bencode.o
	$(CC) $(CFLAGS) -o $@ $^

bencode.o: bencode.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f main.c bencode.o $(GCOV_OUTPUT)
