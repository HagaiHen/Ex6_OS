all: server client test pollserver

server: server.o queue.hpp
	g++ server.o -o server -pthread

pollserver: pollserver.o reactor.hpp
	g++ pollserver.o -o pollserver -pthread

	
client: client.o
	g++ -o client client.o

server.o: server.cpp 
	g++ -c server.cpp

pollserver.o: pollserver.cpp 
	g++ -c pollserver.cpp

client.o: client.cpp 
	g++ -c client.cpp

test: test.o
	g++ test.o -o test -pthread

test.o:
	g++ -c test.cpp 

clear:
	find . -name "*.o" | xargs rm -f server client server client 

.PHONY: clean all

clean:
	rm -rf *.o server client test pollserver