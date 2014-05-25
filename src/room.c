#include <stdlib.h>
#include <string.h>
#include "room.h"
#include "util.h"

Room *roomCreate(char *name){
    Room *room=calloc(1, sizeof(Room));
    
    strcpy(room->name, name);
    pthread_mutex_init(&(room->mutex), PTHREAD_MUTEX_DEFAULT);

    return room;
}

void roomSendMessage(Room *room, Message *message){
	int i;
	CRITICAL_REGION_BEGIN(room->mutex){
		for(i=0; i<ROOM_SIZE; i++){
			if(room->users[i]!=NULL)
				writeTo(room->users[i]->connection, message);
		}
	} CRITICAL_REGION_END(room->mutex)
}

Room *roomJoin(Room *room, User *user){
	int i;
	bool not_found = True;
	Message *message = NULL;

	CRITICAL_REGION_BEGIN(room->mutex){
		for(i=0; i<ROOM_SIZE && not_found; i++){
			if(room->users[i] == user)
				not_found = False;
		}
		if(room->users[i] != user){
			for(i=0; i<ROOM_SIZE && not_found; i++){
				if(room->users[i]==NULL)
					not_found = False;
			}
			if(i>=ROOM_SIZE){

			} else {
				room->users[i] = user;
			}
		}

		if(message != NULL){
			writeTo(user->connection, message);
			MessageDestroy(message);
		}
	} CRITICAL_REGION_END(room->mutex)
	return room;
}

Room *roomLeave(Room *room, User *user){
	int i;
	bool not_found = True;
	Message *message = NULL;

	CRITICAL_REGION_BEGIN(room->mutex){
		for(i=0; i<ROOM_SIZE && not_found; i++){
			if(room->users[i] == user)
				not_found = False;
		}
		if(room->users[i] == user)
			room->users[i] = NULL;

		for(i=0; i<ROOM_SIZE && not_found; i++){
			if(room->users[i] != NULL)
				not_found = False;
		}
	} CRITICAL_REGION_END(room->mutex)
	if(i>=ROOM_SIZE){
		roomDestroy(room);
		return NULL;
	}


	return room;
}

void roomDestroy(Room *room){
	free(room);
}
