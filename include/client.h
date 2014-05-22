#ifndef _ROOM_H
#define _ROOM_H

#include <pthread.h>

#define CLIENT_NAME_SIZE

#define JOIN_COMMAND "\\join"
#define LEAVE_COMMAND "\\leave"
#define RENAME_COMMAND "\\rename"

typedef struct _Client{
    char client[CLIENT_NAME_SIZE];
    pthread_mutex_t crit_region;
    pthread_cond_t available_message;
} Client;

Client *clientCreate(char *name);

Client *clientReadMessage();

Client *clientWriteMessage();

Client *clientJoin();

Client *clientLeave();

#endif
