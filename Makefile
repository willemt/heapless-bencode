GCOV_CCFLAGS = -fprofile-arcs -ftest-coverage
GCOV_OUTPUT = *.gcda *.gcno *.gcov 
CC     = gcc
CCFLAGS = -g -O2 -Wall -Werror -W -I. -fno-omit-frame-pointer -fno-common -fsigned-char $(GCOV_CCFLAGS)

all: test_bencode

main.c:
	sh tests/make-tests.sh tests/test_bencode.c > main.c

test_bencode: main.c bencode.o tests/test_bencode.c tests/CuTest.c
	$(CC) $(CCFLAGS) -Itests -o $@ $^
	./test_bencode
	gcov main.c bencode.c

bencode_consumer: bencode_consumer.c bencode.o
	$(CC) $(CCFLAGS) -o $@ $^

bencode.o: bencode.c
	$(CC) $(CCFLAGS) -c -o $@ $^

clean:
	rm -f main.c bencode.o $(GCOV_OUTPUT)
