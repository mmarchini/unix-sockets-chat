#include <stdlib.h>
#include <string.h>
#include "user.h"

User *userCreate(int id, char *name, Connection *connection){
	User *user = calloc(1, sizeof(User));

	user->id = id;
	strncpy(user->name, name, USERNAME_SIZE);
	user->connection = connection;
	user->room_id = NO_ROOM;

	return user;
}

int userDestroy(User *user){
	closeConnection(user->connection);

	free(user);

	return 0;
}
