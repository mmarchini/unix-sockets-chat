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

pthread_mutex_t user_list_lock;
User *users_list[MAX_CONNECTED_USERS];

void clientExecuteCommand(User *user, Message *message){
    switch (message->message[1]) {
    	case 'j':
    		break;
    	default:
    		break;
    }
}

void clientHandleMessage(User *user, Message *message){
	char time[256];
	int i;
	time_to_string(time, 256, message->timestamp);
    CRITICAL_REGION_BEGIN(user_list_lock) {
        strcpy(message->sender_name, users_list[user->id]->name);

        for(i=0; i<MAX_CONNECTED_USERS; i++){
        	if(users_list[i]!=NULL)
        		writeTo(users_list[i]->connection, message);
        }
    } CRITICAL_REGION_END(user_list_lock)

	printf("%s[%s]: %s\n", message->sender_name, time, message->message);
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

    if(id>=MAX_CONNECTED_USERS)
    	return NULL;

    // Send User ID to Client
    message->sender_id = id;
    writeTo(client_conn, message);

    MessageDestroy(message);

    // Start Client Handlers
	pthread_create(&reciever, NULL, &clientHandlerReciever, (void *)users_list[id]);

	pthread_join(reciever, &return_token);

    CRITICAL_REGION_BEGIN(user_list_lock) {
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

    connection = openServer(port);
    if(connection==NULL)
    	return 7;

    while(whilecond){
		pthread_create(&token, NULL, &serverStartUser, (void *)acceptConnection(connection));
    }
    closeConnection(connection);

    return 0;
}

