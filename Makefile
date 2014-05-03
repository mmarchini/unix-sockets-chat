
SOURCE_FILES=messages
CC=gcc
CC_FLAGS=-Iinclude
OBJ_FOLDER=obj
SRC_FOLDER=src
BIN_FOLDER=bin
SERVER_BIN=server
CLIENT_BIN=client

all: server client
	
server: obj/$(SOURCE_FILES).o
	@mkdir -p $(BIN_FOLDER)
	@$(CC) $(CC_FLAGS) $(OBJ_FOLDER)/$(SOURCE_FILES).o -o $(BIN_FOLDER)/$(SERVER_BIN)

client: obj/$(SOURCE_FILES).o
	@mkdir -p $(BIN_FOLDER)
	@$(CC) $(CC_FLAGS) $(OBJ_FOLDER)/$(SOURCE_FILES).o -o $(BIN_FOLDER)/$(CLIENT_BIN)

obj/$(SOURCE_FILES).o: $(SRC_FOLDER)/$(SOURCE_FILES).c
	@echo "Compiling $(SOURCE_FILES)"
	@mkdir -p $(OBJ_FOLDER) 
	$(CC) $(CC_FLAGS) -c $(SRC_FOLDER)/$(SOURCE_FILES).c -o $(OBJ_FOLDER)/$(SOURCE_FILES).o
