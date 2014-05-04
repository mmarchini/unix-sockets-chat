#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "connection.h"

#define PORT 4000

int main(int argc, char *argv[])
{
    int n;
    char buffer[256];

    Connection *connection = NULL;
    
    if (argc < 2) {
        fprintf(stderr,"usage %s hostname\n", argv[0]);
        exit(0);
    }

    connection = connectToServer(argv[1], PORT);
    while(true){
     
        printf("Enter the message: ");
        bzero(buffer, 256);
        fgets(buffer, 256, stdin);

        writeTo(connection, buffer, strlen(buffer));
        
        readFrom(connection);
        printf("%s\n",connection->buffer);
    }

    
    closeConnection(connection);
    return 0;
}
