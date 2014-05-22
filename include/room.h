#ifndef _ROOM_H
#define _ROOM_H

#include <pthread.h>

#define ROOM_NAME_SIZE

typedef struct _Room{
    char name[ROOM_NAME_SIZE];
    MessagesQueue queue;
    Messages current_messages;
    int current_clients;
    int released_clients;
    pthread_mutex_t crit_region;
    pthread_cond_t available_message;
} Room;

Room *roomCreate(char *name);

Room *roomSignalMessage();

Room *roomReadMessage();

Room *roomWaitMessage();

Room *roomJoin();

Room *roomLeave();

#endif
