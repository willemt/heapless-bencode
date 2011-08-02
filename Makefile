CC     = gcc
CCFLAGS = -g


all: tests

main.c:
	sh make-tests.sh > main.c

tests: main.c bencode.o test_bencode.c CuTest.c main.c
	$(CC) $(CCFLAGS) -o $@ $^
	./tests

bencode.o: bencode.c
	$(CC) $(CCFLAGS) -c -o $@ $^

clean:
	rm -f main.c bencode.o tests
