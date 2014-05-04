#include "messages.h"
#include <stdlib.h>
#include <string.h>

/*
 * Messages Methods
 */

Message *MessageCreate(char *message, char *sender, time_t timestamp) {
    Message *new_message    = calloc(1, sizeof(Message));
    strcpy(new_message->message, message);
    strcpy(new_message->sender,  sender);
    new_message->timestamp  = timestamp;

    return new_message;
}

void MessageDestroy(Message *message) {
    free(message);
}

/*
 * Messages FIFO Methods
 */

MessageFIFO *MessageFIFOCreate(Message *message) {
    MessageFIFO *fifo =  calloc(1, sizeof(MessageFIFO));
    fifo->message = message;
    fifo->next = NULL;

    return fifo;
}

/*
 * Messages Queue Methods
 */

int MessagesQueueInit(MessagesQueue *messages) {
    pthread_mutex_init(&(messages->crit_region), NULL);
    messages->fifo = NULL;

    return 0;
}
int MessagesQueueInsert(MessagesQueue *messages, Message *message) {
    MessageFIFO *fifo;
    MessageFIFO *temp;

    pthread_mutex_lock(&(messages->crit_region));
       if(messages->fifo == NULL) {
           messages->fifo = MessageFIFOCreate(message);
       } else {
            for(fifo = messages->fifo;fifo->next != NULL || fifo->message->timestamp <= message->timestamp;fifo = fifo->next);
            if(fifo->next == NULL){
                fifo->next = MessageFIFOCreate(message);
            }
            else {
                temp = fifo->next;
                fifo->next = MessageFIFOCreate(message);
                fifo->next->next=temp;
            }
       }
    pthread_mutex_unlock(&(messages->crit_region));

    return 0;
}

Message *MessagesQueueRetrieve(MessagesQueue *messages) {
    MessageFIFO *fifo;
    Message *message;

    pthread_mutex_lock(&(messages->crit_region));
       if(messages->fifo == NULL) {
           message = NULL;
       } else {
           message = calloc(1, sizeof(Message));
           memcpy(message, messages->fifo->message, sizeof(Message));
           fifo = messages->fifo;
           messages->fifo = fifo->next;
           MessageDestroy(fifo->message);
           free(fifo);
       }
    pthread_mutex_unlock(&(messages->crit_region));

    return message;
}

int MessagesQueueDestroy(MessagesQueue *messages) {
    MessageFIFO *fifo;
    
    for(fifo = messages->fifo;fifo != NULL;fifo = fifo->next){
        MessageDestroy(fifo->message);
        free(fifo);
    }
    free(messages);

    return 0;
}

