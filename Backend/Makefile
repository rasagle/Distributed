FLAGS = -std=c++11
CPPS = backendmanager.cpp socketserver.cpp person.cpp replica.cpp
HDRS = backendmanager.h person.h replica.h

all: server

server: $(CPPS) $(HDRS)
	g++ $(FLAGS) $(CPPS) $(HDRS) -o server -pthread

clean:
	rm -f server

run:
	./server 13002 true
