CC      = clang++
CFLAGS  = -std=c++11 -stdlib=libc++
RM      = rm -f

default: all

all: server

server: mongoose.o api.o graph.o server.o 
	${CC} ${CFLAGS} -o $@ $^

server.o: server.cpp

mongoose.o: mongoose.c

api.o: api.cpp

graph.o: graph.cpp

valgrind: server
	valgrind -q —-tool=memcheck —leak-check=yes ./server

clean:
	$(RM) server *.o
	