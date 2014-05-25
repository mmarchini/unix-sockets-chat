#include "connection.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

Connection *openServer(int port) {
    Connection *connection = calloc(1, sizeof(Connection));
    
    if ((connection->sockid = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    	printf("ERROR opening socket!\n");
    	free(connection);

    	return NULL;
    }

    
    connection->addr.sin_family = AF_INET;
    connection->addr.sin_port = htons(port);
    connection->addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(connection->addr.sin_zero), 8);     
    
    if (bind(connection->sockid, (struct sockaddr *) &(connection->addr), sizeof(connection->addr)) < 0){
    	printf("ERROR on binding!\n");
    	close(connection->sockid);
		free(connection);

		return NULL;
    }
    MessagesQueueInit(&(connection->_in_buffer));
	MessagesQueueInit(&(connection->_out_buffer));
	connection->is_active = True;
    
    listen(connection->sockid, 5);
    

    return connection;
}

void *connection_handleRecievedMessages(void *args){
	Connection *connection = (Connection *)args;
    char buffer[BUFFER_SIZE];
    int n;

	while(True){
		/* read from the socket */
		n = read(connection->sockid, buffer, BUFFER_SIZE);
		if (n < 0) {
			printf("ERROR reading from socket\n");
			connection->is_active = False;
			return NULL;
		} else if( n == 0) {
			printf("Connection Closed\n");
			connection->is_active = False;
			return NULL;
		}
		else {
			MessagesQueueInsert(&(connection->_in_buffer), (Message *)buffer);
		}
	}

	return NULL;
}

void *connection_handleSentMessages(void *args){
	Connection *connection = (Connection *)args;
	Message *message = NULL;
    char buffer[BUFFER_SIZE];
    int n;
    bool running = True;

	while(running){
		while(message == NULL){
			if(!connection->is_active){
				printf("Connection closed\n");
				return NULL;
			}

			message = MessagesQueueRetrieve(&(connection->_out_buffer));
		}

		/* read from the socket */
		n = write(connection->sockid,(char *)message,BUFFER_SIZE);
		if (n < 0) {
			connection->is_active = False;
			printf("ERROR reading from socket\n");
			return NULL;
		} else if( n == 0) {
			printf("Connection Closed\n");
			connection->is_active = False;
			return NULL;
		}
		MessageDestroy(message);
		message = NULL;
	}

	return NULL;
}

void handleMessagesExchange(Connection *connection){
	pthread_t sender, reciever;

	pthread_create(&reciever, NULL, &connection_handleRecievedMessages, (void *) connection);
	pthread_create(&sender, NULL, &connection_handleSentMessages, (void *) connection);

}

Connection *connectToServer(char *hostname, int port) {
    Connection *connection = NULL;
    struct hostent *server;

    server = gethostbyname(hostname);

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        free(connection);

        return NULL;
    }
    connection = calloc(1, sizeof(Connection));
    
    if ((connection->sockid = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    	fprintf(stderr, "ERROR opening socket\n");
        free(connection);

        return NULL;
    }

    
    connection->addr.sin_family = AF_INET;     
    connection->addr.sin_port = htons(port);    
    connection->addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(connection->addr.sin_zero), 8);     
    
    if (connect(connection->sockid,(struct sockaddr *) &connection->addr,sizeof(connection->addr)) < 0){
    	fprintf(stderr, "ERROR connecting\n");
    	close(connection->sockid);
        free(connection);

        return NULL;
    }
    MessagesQueueInit(&(connection->_in_buffer));
	MessagesQueueInit(&(connection->_out_buffer));
	connection->is_active = True;

	handleMessagesExchange(connection);
    
    return connection;
}



Connection *acceptConnection(Connection *connection) {
    Connection *client_conn = calloc(1, sizeof(Connection));
    socklen_t clilen;

    clilen = sizeof(struct sockaddr_in);
    if ((client_conn->sockid = accept(connection->sockid, (struct sockaddr *) &(client_conn->addr), &clilen)) == -1){
    	printf("ERROR on accept");
    	free(connection);

    	return NULL;
    }
    MessagesQueueInit(&(connection->_in_buffer));
	MessagesQueueInit(&(connection->_out_buffer));
	client_conn->is_active = True;

	handleMessagesExchange(client_conn);
    
    return client_conn;
}

Message *readFrom(Connection *remote){
    int n;
    Message *message = NULL;

    /* read from the socket */
    while(message == NULL){
    	if(!remote->is_active){
    		printf("[readFrom]Connection closed\n");
    		return NULL;
    	}

		message = MessagesQueueRetrieve(&(remote->_in_buffer));
	}

    return message;
}

int writeTo(Connection *remote, Message *message){
    /* write in the socket */
	if(!remote->is_active)
		return -1;
    MessagesQueueInsert(&(remote->_out_buffer), message);

    return 0;
}

void closeConnection(Connection *connection){
    close(connection->sockid);
    free(connection);
}


