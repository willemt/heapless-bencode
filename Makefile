GCOV_CCFLAGS = -fprofile-arcs -ftest-coverage
GCOV_OUTPUT = *.gcda *.gcno *.gcov 
CC     = gcc
CCFLAGS = -g -O2 -Wall -Werror -W -fno-omit-frame-pointer -fno-common -fsigned-char $(GCOV_CCFLAGS)

all: tests 

main.c:
	sh make-tests.sh > main.c

tests: main.c bencode.o test_bencode.c CuTest.c
	$(CC) $(CCFLAGS) -o $@ $^
	./tests
	gcov main.c test_bencode.c bencode.c

bencode_consumer: bencode_consumer.c bencode.o
	$(CC) $(CCFLAGS) -o $@ $^

bencode.o: bencode.c
	$(CC) $(CCFLAGS) -c -o $@ $^

clean:
	rm -f main.c bencode.o tests $(GCOV_OUTPUT)
