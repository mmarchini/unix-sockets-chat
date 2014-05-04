#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "connection.h"
#include "messages.h"
#include "util.h"

#define PORT 4000

int main(int argc, char *argv[])
{
    int n;
    Message message;

    Connection *connection = NULL;
    
    if (argc < 3) {
        fprintf(stderr,"usage %s hostname username\n", argv[0]);
        exit(0);
    }

    connection = connectToServer(argv[1], PORT);

    strcpy(message.sender, argv[2]);

    while(true){

        printf("%s: ", message.sender);
        bzero((char *)&message, BUFFER_SIZE);
        fgets(message.message, MESSAGE_SIZE, stdin);
        strcpy(message.sender, argv[2]);
        message.timestamp = timestamp();

        writeTo(connection, (char *)&message, BUFFER_SIZE);
        
//      readFrom(connection);

//      printf("%s\n",connection->buffer);
    }

    
    closeConnection(connection);
    return 0;
}
