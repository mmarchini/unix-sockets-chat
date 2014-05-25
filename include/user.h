#ifndef _USER_H
#define _USER_H

#include <pthread.h>
#include "connection.h"
#include "constants.h"

typedef struct _User{
	int id;
    char name[USERNAME_SIZE];
    int room_id;
    Connection *connection;
} User;

User *userCreate(int id, char *name, Connection *connection);

int userDestroy(User *user);

#endif
