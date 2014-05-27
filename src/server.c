#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "connection.h"  
#include "messages.h"
#include "util.h"
#include "constants.h"
#include "user.h"
#include "room.h"

#define CREATE_COMMAND "\\create"
#define JOIN_COMMAND "\\join"
#define LEAVE_COMMAND "\\leave"
#define RENAME_COMMAND "\\name"
#define ROOMS_COMMAND "\\rooms"
#define USERS_COMMAND "\\users"
#define EXIT_COMMAND "\\exit"

pthread_mutex_t user_list_lock;
User *users_list[MAX_CONNECTED_USERS];

pthread_mutex_t room_list_lock;
Room *rooms_list[MAX_SIMULTANEOUS_ROOMS];

void clientExecuteCommand(User *user, Message *message){
	Message *msg = NULL;
	int room_id, user_id;
	char *token = NULL;
	char token2[MESSAGE_SIZE];
	char token3[MESSAGE_SIZE];
	bool not_found = True;

	token = strtok(message->message, " ");
	if(!strcmp(token, JOIN_COMMAND)) {
		token = strtok(NULL, "\n");
		CRITICAL_REGION_BEGIN(room_list_lock) {
			for(room_id=0;room_id<MAX_SIMULTANEOUS_ROOMS && not_found;room_id++){
				if (rooms_list[room_id] != NULL)
					if (!strcmp(token, rooms_list[room_id]->name))
						not_found = False;
			}

			if (not_found == False){
				room_id--;
				if(user->room_id != NO_ROOM)
					rooms_list[user->room_id] = roomLeave(rooms_list[user->room_id], user);
				rooms_list[room_id] = roomJoin(rooms_list[room_id], user);
				user->room_id = room_id;

				sprintf(token2, "Joining room %s [%d]", token, room_id);
				msg = MessageCreate(token2, "*System*", -1, timestamp());
			} else{
				msg = MessageCreate("Room not found", "*System*", -1, timestamp());
			}
		} CRITICAL_REGION_END(room_list_lock)

	} else if(!strcmp(token, CREATE_COMMAND)) {
		token = strtok(NULL, "\n");
		CRITICAL_REGION_BEGIN(room_list_lock) {
			for(room_id=0;room_id<MAX_SIMULTANEOUS_ROOMS && not_found;room_id++){
				if (rooms_list[room_id] != NULL)
					if (!strcmp(token, rooms_list[room_id]->name))
						not_found = False;
			}
			if(not_found == False){
				msg = MessageCreate("A room with this name already exists", "*System*", -1, timestamp());
			} else {
				not_found = True;
				for(room_id=0;room_id<MAX_SIMULTANEOUS_ROOMS && not_found;room_id++){
					if (rooms_list[room_id] == NULL)
						not_found = False;
				}

				if(not_found){
					msg = MessageCreate("Can't create more rooms!", "*System*", -1, timestamp());
				} else  {
					room_id--;
					if(user->room_id != NO_ROOM)
						rooms_list[user->room_id] = roomLeave(rooms_list[user->room_id], user);
					rooms_list[room_id] = roomCreate(token);
					rooms_list[room_id] = roomJoin(rooms_list[room_id], user);
					user->room_id = room_id;
					sprintf(token2, "Creating room %s [%d]", token, room_id);
					msg = MessageCreate(token2, "*System*", -1, timestamp());
				}
			}
		} CRITICAL_REGION_END(room_list_lock)
	} else if(!strcmp(token, RENAME_COMMAND)) {
		token = strtok(NULL, "\n");
		strcpy(user->name, token);
	} else {
		token = strtok(message->message, "\n");

		if(!strcmp(token, EXIT_COMMAND)) {
			user->connection->is_active = False;
		} else if(!strcmp(token, LEAVE_COMMAND)) {
			CRITICAL_REGION_BEGIN(room_list_lock) {
				if(user->room_id != NO_ROOM){
					sprintf(token2, "Leaving room %s [%d]", rooms_list[user->room_id]->name, user->room_id);
					msg = MessageCreate(token2, "*System*", -1, timestamp());
					rooms_list[user->room_id] = roomLeave(rooms_list[user->room_id], user);
				} else{
					msg = MessageCreate("Error trying to leave room", "*System*", -1, timestamp());
				}
			} CRITICAL_REGION_END(room_list_lock)
		} else if(!strcmp(token, ROOMS_COMMAND)) {
			CRITICAL_REGION_BEGIN(room_list_lock) {
				bzero(token2, MESSAGE_SIZE);
				bzero(token3, MESSAGE_SIZE);
				for(room_id=0;room_id<MAX_SIMULTANEOUS_ROOMS;room_id++){
					if(rooms_list[room_id]!=NULL){
						sprintf(token2, "%s '%s'", token3, rooms_list[room_id]->name);
						sprintf(token3, "%s", token2);
					}
				}
				sprintf(token3, "Rooms list:%s", token2);
				msg = MessageCreate(token3, "*System*", -1, timestamp());
			} CRITICAL_REGION_END(room_list_lock)
		} else if(!strcmp(token, USERS_COMMAND)) {
			CRITICAL_REGION_BEGIN(room_list_lock) {
				bzero(token2, MESSAGE_SIZE);
				bzero(token3, MESSAGE_SIZE);
				for(user_id=0;user_id<ROOM_SIZE;user_id++){
					if(rooms_list[user->room_id]->users[user_id]!=NULL){
						sprintf(token2, "%s '%s'", token3, rooms_list[user->room_id]->users[user_id]->name);
						sprintf(token3, "%s", token2);
					}

				}
				sprintf(token3, "Users in room:%s", token2);
				msg = MessageCreate(token3, "*System*", -1, timestamp());
			} CRITICAL_REGION_END(room_list_lock)
		}
		else{
			msg = MessageCreate("Invalid command", "*System*", -1, timestamp());
		}
	}

	if(msg != NULL){
		writeTo(user->connection, msg);
		MessageDestroy(msg);
	}
}

void clientHandleMessage(User *user, Message *message){
	char time[256];
	int i;
	time_to_string(time, 256, message->timestamp);
    CRITICAL_REGION_BEGIN(user_list_lock) {
        strcpy(message->sender_name, user->name);
        message->room_id = user->room_id;

        if(user->room_id != NO_ROOM)
        	CRITICAL_REGION_BEGIN(room_list_lock){
        		roomSendMessage(rooms_list[user->room_id], message);
        	} CRITICAL_REGION_END(room_list_lock)
    } CRITICAL_REGION_END(user_list_lock)

	printf("%s (%d) [%s]: %s\n", message->sender_name, message->room_id, time, message->message);
}

void *clientHandlerReciever(void *args){
    User *user = (User *)args;
    Message *message;
    bool running=True;

    while(running){
        message = readFrom(user->connection);
        if(message != NULL){
        	switch (message->message[0]) {
				case '\\':
					clientExecuteCommand(user, message);
					break;
				default:
					clientHandleMessage(user, message);
					break;
			}

			MessageDestroy(message);
        }
        else
        	running = False;
	}

    return NULL;
}

void *serverStartUser(void *args){
	Connection *client_conn = (Connection *)args;
	pthread_t sender, reciever;
	Message *message;
    int id;
    void *return_token;

    // Get User Name from Client
    if(client_conn==NULL){
    	printf("Connection error\n");
    	return NULL;
    }

    message = readFrom(client_conn);
    if(message==NULL){
    	printf("Weird error\n");
    	return NULL;
    }

    // Get User ID for Client
    CRITICAL_REGION_BEGIN(user_list_lock) {
    	for(id=0; id < MAX_CONNECTED_USERS && users_list[id]!=NULL; id++);
    	if(id<MAX_CONNECTED_USERS)
    		users_list[id]=userCreate(id, message->sender_name, client_conn);
    } CRITICAL_REGION_END(user_list_lock)

    if(id>=MAX_CONNECTED_USERS){
        client_conn->is_active=False;
        return NULL;
    }

    // Send User ID to Client
    message->sender_id = id;
    writeTo(client_conn, message);

    MessageDestroy(message);

    // Start Client Handlers
	pthread_create(&reciever, NULL, &clientHandlerReciever, (void *)users_list[id]);

	pthread_join(reciever, &return_token);

    CRITICAL_REGION_BEGIN(user_list_lock) {
    	if(users_list[id]->room_id != NO_ROOM)
    		CRITICAL_REGION_BEGIN(room_list_lock) {
    			rooms_list[users_list[id]->room_id] = roomLeave(rooms_list[users_list[id]->room_id], users_list[id]);
    		} CRITICAL_REGION_END(room_list_lock)
    	userDestroy(users_list[id]);
        users_list[id]=NULL;
    } CRITICAL_REGION_END(user_list_lock)

    return NULL;
}

int startServer(int port) {
	pthread_t token;
    Connection *connection;
    Connection *client_conn;

    int i;
    bool whilecond = True;

    signal(SIGPIPE, SIG_IGN);

    pthread_mutex_init(&user_list_lock, PTHREAD_MUTEX_DEFAULT);

    CRITICAL_REGION_BEGIN(user_list_lock) {
    	for(i=0;i<MAX_CONNECTED_USERS;i++) users_list[i] = NULL;
    } CRITICAL_REGION_END(user_list_lock)

    CRITICAL_REGION_BEGIN(room_list_lock) {
		for(i=0;i<MAX_SIMULTANEOUS_ROOMS;i++) rooms_list[i] = NULL;
	} CRITICAL_REGION_END(room_list_lock)

    connection = openServer(port);
    if(connection==NULL)
    	return 7;

    while(whilecond){
		pthread_create(&token, NULL, &serverStartUser, (void *)acceptConnection(connection));
    }
    closeConnection(connection);

    return 0;
}

