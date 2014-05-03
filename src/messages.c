#include "messages.h"
#include <stdlib.h>
#include <string.h>

int MessagesInit(Messages *messages) {
    pthread_mutex_init(&(messages->crit_region), NULL);
    messages->fifo = NULL;

    return 0;
}

MessageFIFO *MessageFIFOCreate(char *message) {
    MessageFIFO *fifo =  calloc(1, sizeof(MessageFIFO));
    fifo->message = message;
    fifo->next = NULL;

    return fifo;
}

int MessagesInsert(Messages *messages, char *message) {
    MessageFIFO *fifo;

    pthread_mutex_lock(&(messages->crit_region));
       if(messages->fifo == NULL) {
           messages->fifo = MessageFIFOCreate(message);
       } else {
            for(fifo = messages->fifo;fifo->next != NULL;fifo = fifo->next);
            fifo->next = MessageFIFOCreate(message);
       }
    pthread_mutex_unlock(&(messages->crit_region));

    return 0;
}

char *MessagesRetrieve(Messages *messages) {
    MessageFIFO *fifo;
    char *message;

    pthread_mutex_lock(&(messages->crit_region));
       if(messages->fifo == NULL) {
           message = NULL;
       } else {
           message = calloc(strlen(messages->fifo->message), sizeof(char));
           strcpy(message, messages->fifo->message);
           fifo = messages->fifo;
           messages->fifo = fifo->next;
           free(fifo->message);
           free(fifo);
       }
    pthread_mutex_unlock(&(messages->crit_region));

    return message;
}

int MessagesDestroy(Messages *messages) {
    MessageFIFO *fifo;
    
    for(fifo = messages->fifo;fifo != NULL;fifo = fifo->next){
        free(fifo->message);
        free(fifo);
    }
    free(messages);

    return 0;
}

