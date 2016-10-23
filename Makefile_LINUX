CC      = g++
CFLAGS  = -std=c++0x
RM      = rm -f

default: all

all: cs426_graph_server

cs426_graph_server: mongoose.o log.o checkpoint.o api.o graph.o cs426_graph_server.o 
	${CC} ${CFLAGS} -o $@ $^

cs426_graph_server.o: cs426_graph_server.cpp

mongoose.o: mongoose.c

api.o: api.cpp

graph.o: graph.cpp

log.o: log.cpp

checkpoint.o: checkpoint.cpp

valgrind: cs426_graph_server
	valgrind -q —-tool=memcheck —leak-check=yes ./cs426_graph_server

clean:
	$(RM) cs426_graph_server *.o
	