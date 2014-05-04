
UTIL_FILES = messages connection util
CC = gcc
CC_FLAGS = -Iinclude -lpthread -g
OBJ_FOLDER = obj
OBJ_FILES  := ${foreach file, $(UTIL_FILES),$(OBJ_FOLDER)/$(file).o}
SRC_FOLDER = src
SRC_FILES  := ${foreach file, $(UTIL_FILES),$(SRC_FOLDER)/$(file).c}
BIN_FOLDER = bin

SERVER_BIN = server
CLIENT_BIN = client

all: $(SERVER_BIN) $(CLIENT_BIN)
	@echo "All compiled!"
	
$(SERVER_BIN): $(OBJ_FILES) 
	@echo "Compiling $(SERVER_BIN)"
	@mkdir -p $(BIN_FOLDER)
	$(CC) $(CC_FLAGS) -c $(SRC_FOLDER)/$(SERVER_BIN).c -o $(OBJ_FOLDER)/$(SERVER_BIN).o
	$(CC) $(CC_FLAGS) $(OBJ_FILES) $(OBJ_FOLDER)/$(SERVER_BIN).o -o $(BIN_FOLDER)/$(SERVER_BIN)

$(CLIENT_BIN): $(OBJ_FILES)
	@echo "Compiling $(CLIENT_BIN)"
	@mkdir -p $(BIN_FOLDER)
	$(CC) $(CC_FLAGS) -c $(SRC_FOLDER)/$(CLIENT_BIN).c -o $(OBJ_FOLDER)/$(CLIENT_BIN).o
	$(CC) $(CC_FLAGS) $(OBJ_FILES) $(OBJ_FOLDER)/$(CLIENT_BIN).o -o $(BIN_FOLDER)/$(CLIENT_BIN)

$(OBJ_FILES): 
	${eval curfile=${subst .o,.c,${subst obj,src,$@}}}
	@echo "Compiling $(curfile) into $@"
	@mkdir -p $(OBJ_FOLDER) 
	@$(CC) $(CC_FLAGS) -c $(curfile) -o $@

clean:
	rm -rf obj bin
