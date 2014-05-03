#ifndef _MESSAGES_H
#define _MESSAGES_H
#include <pthread.h>

typedef struct _MessageFIFO {
    char *message;
    struct _MessageFIFO *next;
} MessageFIFO;

typedef struct _Messages{
    pthread_mutex_t crit_region;
    MessageFIFO *fifo;
} Messages;

int MessagesInit(Messages *messages);

int MessagesInsert(Messages *messages, char *message);

char *MessagesRetrieve(Messages *messages);

int MessagesDestroy(Messages *messages);

#endif
