#ifndef _CONNECTION_H
#define _CONNECTION_H

#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 256

#define true  1
#define false 0

typedef struct _Connection {
    int sockid;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in addr;
} Connection;

Connection *openServer(int port);

Connection *connectToServer(char *hostname, int port); 

Connection *acceptConnection(Connection *connection); 

void readFrom(Connection *remote);

void writeTo(Connection *remote, char *msg, int len);

#endif
