#include "messages.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

/*
 * Messages Methods
 */

Message *MessageCreate(char *message, char *sender_name, int sender_id, time_t timestamp) {
    Message *new_message    = calloc(1, sizeof(Message));
    strcpy(new_message->message, message);
    strcpy(new_message->sender_name,  sender_name);
    new_message->sender_id = sender_id;
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
    fifo->message = calloc(1, sizeof(Message));
    memcpy(fifo->message, message, sizeof(Message));
    fifo->next = NULL;

    return fifo;
}

/*
 * Messages Queue Methods
 */

int MessagesQueueInit(MessagesQueue *messages) {
    pthread_mutex_init(&(messages->crit_region), PTHREAD_MUTEX_DEFAULT);
    messages->fifo = NULL;

    return 0;
}
int MessagesQueueInsert(MessagesQueue *messages, Message *message) {
    MessageFIFO *fifo;
    MessageFIFO *temp;

    CRITICAL_REGION_BEGIN((messages->crit_region)){
    	if(messages->fifo == NULL) {
    		messages->fifo = MessageFIFOCreate(message);
    	} else {
    		for(fifo = messages->fifo;fifo->next != NULL;fifo = fifo->next);
    		if(fifo->next == NULL){
				fifo->next = MessageFIFOCreate(message);
			}
			else {
				temp = fifo->next;
				fifo->next = MessageFIFOCreate(message);
				fifo->next->next=temp;
			}
	   }
    }CRITICAL_REGION_END((messages->crit_region))


    return 0;
}

Message *MessagesQueueRetrieve(MessagesQueue *messages) {
    MessageFIFO *fifo;
    Message *message;

    CRITICAL_REGION_BEGIN((messages->crit_region)){
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
    }CRITICAL_REGION_END((messages->crit_region))

    return message;
}

int MessagesQueueDestroy(MessagesQueue *messages) {
    MessageFIFO *fifo;
    
    CRITICAL_REGION_BEGIN((messages->crit_region)){
		for(fifo = messages->fifo;fifo != NULL;fifo = fifo->next){
			//MessageDestroy(fifo->message);
			//free(fifo);
		}
    }CRITICAL_REGION_END((messages->crit_region))
    free(messages);

    return 0;
}

