BIN = ./bin
SRC = ./src
OBJ = ./obj

CC = g++

server_bin = $(BIN)/server
server_src = $(SRC)/server.cpp
server_obj = $(OBJ)/server.o

client_bin = $(BIN)/client
client_src = $(SRC)/client.cpp
client_obj = $(OBJ)/client.o


$(shell if [ ! -e $(OBJ) ];then mkdir -p $(OBJ); fi)
$(shell if [ ! -e $(BIN) ];then mkdir -p $(BIN); fi)

CFLAGS = -I /opt/homebrew/Cellar/mysql++/3.3.0_1/include/mysql++ -I /opt/homebrew/Cellar/mysql/8.0.27/include/mysql -pthread -std=c++20 -g

build : $(server_obj) $(client_obj)
	$(CC) $(CFLAGS) -o $(server_bin) $(server_obj)
	$(CC) $(CFLAGS) -o $(client_bin) $(client_obj)

$(server_obj) : $(server_src)
	$(CC) $(CFLAGS) -o $(server_obj) -c $(server_src)

$(client_obj) : $(client_src)
	$(CC) $(CFLAGS) -o $(client_obj) -c $(client_src)


clean:
	rm -rf ${BIN} ${OBJ}