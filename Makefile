prog3: prog3.o List.h List.o Dictionary.h Dictionary.o HashTable.h HashTable.o
	gcc -g -std=c99 -o prog3 prog3.o List.o Dictionary.o HashTable.o

%.o: %.c
	gcc -g -std=c99 -c -o $@ $<

clean:
	rm prog3 *.o
