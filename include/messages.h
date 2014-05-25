#ifndef _MESSAGES_H
#define _MESSAGES_H
#include <pthread.h>
#include <time.h>
#include "constants.h"

/*
 * Message
 */

typedef struct _Message {
    char message[MESSAGE_SIZE];
    char sender_name[USERNAME_SIZE];
    int sender_id;
    char room[ROOM_SIZE];
    time_t timestamp;
} Message;

Message *MessageCreate(char *message, char *sender_name, int sender_id, time_t timestamp);

void MessageDestroy(Message *message);

/*
 * MessageFIFO
 */

typedef struct _MessageFIFO {
    Message *message;
    struct _MessageFIFO *next;
} MessageFIFO;

/*
 * MessageQueue
 */

typedef struct _MessagesQueue{
    pthread_mutex_t crit_region;
    MessageFIFO *fifo;
} MessagesQueue;

int MessagesQueueInit(MessagesQueue *messages);

int MessagesQueueInsert(MessagesQueue *messages, Message *message);

Message *MessagesQueueRetrieve(MessagesQueue *messages);

int MessagesQueueDestroy(MessagesQueue *messages);

#endif
