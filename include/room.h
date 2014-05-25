#ifndef _ROOM_H
#define _ROOM_H

#include <pthread.h>
#include "user.h"
#include "messages.h"

typedef struct _Room{
    char name[ROOM_NAME_SIZE];
    User *users[ROOM_SIZE];

    pthread_mutex_t mutex;
} Room;

Room *roomCreate(char *name);

void roomSendMessage(Room *room, Message *message);

Room *roomJoin(Room *room, User *user);

Room *roomLeave(Room *room, User *user);

void roomDestroy(Room *room);

#endif
