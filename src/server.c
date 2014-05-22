#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "connection.h"  
#include "messages.h"
                         
#define PORT 4000        

void *clientReciever(void *args){
    Connection *client = (Connection *)args;
    Message message;
    char time[256];
    while(true){
        readFrom(client);
        memcpy((char *)&message, client->buffer, sizeof(Message));
        bzero(time, 256);
        strftime(time, 256, "%d/%m/%Y %H:%M", localtime(&message.timestamp));

        printf("%s[%s]: %s\n", message.sender, time, message.message);
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
        pthread_create(&token, NULL, &clientReciever, (void *) client_conn);
    }
    readFrom(client_conn);

    printf("Here is the message: %s\n", client_conn->buffer);
    
    writeTo(client_conn, "I got your message", 18);
    
    closeConnection(connection);
    closeConnection(client_conn);
    return 0; 
}
