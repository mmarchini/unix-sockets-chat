#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "connection.h"

#define PORT 4000

void *clientHandler(void *args){
    Connection *client = (Connection *)args;
    while(true){
        readFrom(client);

        printf("Here is the message: %s\n", client->buffer);
        
        writeTo(client, "I got your message", 18);
    }
}

int main(int argc, char *argv[])
{
    int n;
    pthread_t token;
    Connection *connection;
    Connection *client_conn;

    connection = openServer(PORT);

    while(true){
        client_conn = acceptConnection(connection);
        pthread_create(&token, NULL, &clientHandler, (void *) client_conn);
    }
    readFrom(client_conn);

    printf("Here is the message: %s\n", client_conn->buffer);
    
    writeTo(client_conn, "I got your message", 18);
    
    closeConnection(connection);
    closeConnection(client_conn);
    return 0; 
}
