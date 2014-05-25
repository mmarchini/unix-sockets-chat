#ifndef _CONNECTION_H
#define _CONNECTION_H

#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "messages.h"
#include "util.h"

#define BUFFER_SIZE sizeof(Message)

typedef struct _Connection {
    int sockid;
    struct sockaddr_in addr;

    bool is_active;

    MessagesQueue _in_buffer;
    MessagesQueue _out_buffer;
} Connection;

Connection *openServer(int port);

Connection *connectToServer(char *hostname, int port); 

Connection *acceptConnection(Connection *connection); 

Message *readFrom(Connection *remote);

int writeTo(Connection *remote, Message *message);

#endif
