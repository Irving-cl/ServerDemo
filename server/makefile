
echo_server: server.o
	g++ -o bin/echo_server server.o
server.o: server.cpp
	g++ -c server.cpp -g
clean:
	rm -f *.o echo_server
