#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include "connection.h"
#include "messages.h"
#include "util.h"
#include "constants.h"

int user_id;

void *handleSentMessages(void *args){
    Connection *connection = (Connection *)args;
    Message *message;
    char buffer[MESSAGE_SIZE];
    char time[256];
    bool running=True;
    if(connection == NULL){
    	printf("o.O");
    }

    while(running){
        printf("> ");
        bzero(buffer, MESSAGE_SIZE);
        fgets(buffer, MESSAGE_SIZE, stdin);

        message = MessageCreate(buffer, "", user_id, timestamp());

        if(writeTo(connection, message) != 0 || !connection->is_active)
        	running=False;
    }

    return NULL;
}

void *handleRecieveMessages(void *args){
    Connection *connection = (Connection *)args;
    Message *message;
    char time[256];
    bool running = True;

    while(running){
    	message = readFrom(connection);
    	if(message != NULL && connection->is_active){
			bzero(time, 256);
			strftime(time, 256, "%d/%m/%Y %H:%M", localtime(&message->timestamp));

			printf("%s[%s]: %s\n", message->sender_name, time, message->message);
			MessageDestroy(message);
    	} else {
    		running=False;
    	}
    }

    return NULL;
}

int startClient(char *ip, int port, char *user_name) {
    pthread_t sender, reciever;
    void *reciever_return, *sender_return;
    Connection *connection= connectToServer(ip, port);
    char zerobuffer[MESSAGE_SIZE];
    if(connection == NULL)
    	return 1;
    bzero(zerobuffer, MESSAGE_SIZE);
    Message *message = MessageCreate(zerobuffer, user_name, -1, timestamp());

    writeTo(connection, message);

    MessageDestroy(message);

    message = readFrom(connection);
    user_id = message->sender_id;

    pthread_create(&reciever, NULL, &handleRecieveMessages, (void *) connection);
    pthread_create(&sender, NULL, &handleSentMessages, (void *) connection);

    pthread_join(reciever, &reciever_return);
    pthread_join(sender, &sender_return);
    
    closeConnection(connection);

    return 0;
}
