BIN = ./bin
SRC = ./src
OBJ = ./obj

CC = g++

server_bin = $(BIN)/server
server_src = $(SRC)/server.cpp
server_obj = $(OBJ)/server.o

cilent_bin = $(BIN)/cilent
cilent_src = $(SRC)/cilent.cpp
cilent_obj = $(OBJ)/cilent.o


$(shell if [ ! -e $(OBJ) ];then mkdir -p $(OBJ); fi)
$(shell if [ ! -e $(BIN) ];then mkdir -p $(BIN); fi)

CFLAGS = -I /opt/homebrew/Cellar/mysql++/3.3.0_1/include/mysql++ -I /opt/homebrew/Cellar/mysql/8.0.27/include/mysql

build : $(server_obj) $(cilent_obj)
	$(CC) $(CFLAGS) -o $(server_bin) $(server_obj)
	$(CC) $(CFLAGS) -o $(cilent_bin) $(cilent_obj)

$(server_obj) : $(server_src)
	$(CC) -o $(server_obj) -c $(server_src)

$(cilent_obj) : $(cilent_src)
	$(CC) -o $(cilent_obj) -c $(cilent_src)


clean:
	rm -rf ${BIN} ${OBJ}