CC        = g++
CFLAGS    = -Wall -pedantic -std=c++11 -Wextra
LDFLAGS   = -lboost_system -pthread
CPP_FILES = server.cpp client.cpp
OBJ_FILES = $(CPP_FILES:.cpp=.o)
SRVR_BIN  = server
CLNT_BIN  = client
BIN_FILES = $(SRVR_BIN) $(CLNT_BIN)

all: eaton_task clean

eaton_task: server client

$(BIN_FILES): %: %.o
	$(CC) -o $@ $^ $(LDFLAGS)


$(OBJ_FILES): %.o: %.cpp
	$(CC) -c $(CFLAGS) $<

.PHONY: clean cleanall

clean:
	rm -f *.o

cleanall: clean
	rm -f server
	rm -f client
