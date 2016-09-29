
server-exe: server.o
	gcc -o server-exe server.o
server.o: server.c ../include/def.h
	gcc -c server.c
clean:
	rm -f *.o server-exe
