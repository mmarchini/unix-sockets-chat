
LIB_FILES = messages connection util server client user room
CC = gcc
CC_FLAGS = -Iinclude -lpthread -g -lncurses 
OBJ_FOLDER = obj
OBJ_FILES  := ${foreach file, $(LIB_FILES),$(OBJ_FOLDER)/$(file).o}
SRC_FOLDER = src
SRC_FILES  := ${foreach file, $(LIB_FILES),$(SRC_FOLDER)/$(file).c}
BIN_FOLDER = bin


SERVER_BIN = server
CLIENT_BIN = client

all: $(SERVER_BIN) $(CLIENT_BIN)
	@echo "All compiled!"
	
$(SERVER_BIN): $(OBJ_FILES) 
	@echo "Compiling $(SERVER_BIN)"
	@mkdir -p $(BIN_FOLDER)
	@$(CC) -c $(SRC_FOLDER)/main_$(SERVER_BIN).c -o $(OBJ_FOLDER)/main_$(SERVER_BIN).o $(CC_FLAGS)
	@$(CC) $(OBJ_FILES) $(OBJ_FOLDER)/main_$(SERVER_BIN).o -o $(BIN_FOLDER)/$(SERVER_BIN) $(CC_FLAGS)

$(CLIENT_BIN): $(OBJ_FILES)
	@echo "Compiling $(CLIENT_BIN)"
	@mkdir -p $(BIN_FOLDER)
	@$(CC) -c $(SRC_FOLDER)/main_$(CLIENT_BIN).c -o $(OBJ_FOLDER)/main_$(CLIENT_BIN).o $(CC_FLAGS)
	@$(CC) $(OBJ_FILES) $(OBJ_FOLDER)/main_$(CLIENT_BIN).o -o $(BIN_FOLDER)/$(CLIENT_BIN) $(CC_FLAGS)

$(OBJ_FILES): $(SRC_FILES)
	${eval curfile=${subst .o,.c,${subst obj,src,$@}}}
	@echo "Compiling $(curfile) into $@"
	@mkdir -p $(OBJ_FOLDER) 
	@$(CC) -c $(curfile) -o $@ $(CC_FLAGS)

clean:
	rm -rf obj bin

