CC     = gcc
GCOV_CCFLAGS = -fprofile-arcs -ftest-coverage
CCFLAGS = -g -O2 -Wall -Werror -W -fno-omit-frame-pointer -fno-common -fsigned-char $(GCOV_CCFLAGS)
GCOV_OUTPUT = main.gcda main.gcno test_bencode.gcno test_bencode.gcda bencode.gcda bencode.gcno

all: tests

main.c:
	sh make-tests.sh > main.c

tests: main.c bencode.o test_bencode.c CuTest.c
	$(CC) $(CCFLAGS) -o $@ $^
	./tests
	gcov main.c test_bencode.c bencode.c

bencode.o: bencode.c
	$(CC) $(CCFLAGS) -c -o $@ $^

clean:
	rm -f main.c bencode.o tests *.gcda *.gcno *.gcov
	#$(GCOV_OUTPUT)
