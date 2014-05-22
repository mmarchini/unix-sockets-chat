#include "room.h"

Room *roomCreate(char *name){
    Room *room=calloc(1, sizeof(Room));
    
    strcpy(room->name, name);

    MessagesQueueInit(&(room->queue));

    room->current_messages = NULL;

    room->current_clients=0;
    room->released_clients=0;
    pthread_cond_init(&available_message, NULL);

    return room;
}

void roomThrowMessage(Room *room){
}

void roomWaitMessage(Room *room){
    pthread_mutex_lock(&(room->crit_region)); 
    pthread_cond_wait(&(room->available_message), &(room->crit_region)); 
}

void roomReadMessage(Room *room, Message *message){
    roomWaitMessage; 
    {
        memcpy(message, room->current_message, sizeof(Message));
        room->released_clients--;
    }
}

int roomJoin(Room *room){
    room->current_clients++;
}

int roomLeave(Room *room){
    room->current_clients--;
}

