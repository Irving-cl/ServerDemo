
client-exe: client.o
	gcc -o client-exe client.o
client.o: client.c ../include/def.h
	gcc -c client.c
clean:
	rm -f *.o client-exe
