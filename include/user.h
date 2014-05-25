#ifndef _USER_H
#define _USER_H

#include <pthread.h>
#include "connection.h"
#include "constants.h"

#define JOIN_COMMAND "\\join"
#define LEAVE_COMMAND "\\leave"
#define RENAME_COMMAND "\\rename"

typedef struct _User{
	int id;
    char name[USERNAME_SIZE];
    Connection *connection;
} User;

User *userCreate(int id, char *name, Connection *connection);

int userDestroy(User *user);

#endif
