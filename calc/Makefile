DIR_INC = ./include
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin

FSQL = $(mysql_config --cflags)
LIBS = $(mysql_config --libs)

SQL_LIBS = -L/usr/lib/x86_64-linux-gnu -lmysqlclient
SQL_CFLAGS = -I/usr/include/mysql

SRC = $(wildcard ${DIR_SRC}/*.cpp)
OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC}))

TARGET = calc

BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = mpic++
CFLAGS = -g -Wall -I${DIR_INC}

${BIN_TARGET}:${OBJ}
	$(CC) $(OBJ) -o $@ $(SQL_LIBS) $(SQL_CFLAGS)

${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	$(CC) $(CFLAGS) -o $@ $(FSQL) -c $< $(LIBS)
.PHONY:clean
clean:
	find ${DIR_OBJ} -name *.o -exec rm -rf {} \;
