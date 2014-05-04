#include "connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h> 

Connection *openServer(int port) {
    Connection *connection = calloc(1, sizeof(Connection));
    
    if ((connection->sockid = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        printf("ERROR opening socket");
    
    connection->addr.sin_family = AF_INET;
    connection->addr.sin_port = htons(port);
    connection->addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(connection->addr.sin_zero), 8);     
    
    if (bind(connection->sockid, (struct sockaddr *) &(connection->addr), sizeof(connection->addr)) < 0) 
        printf("ERROR on binding");
    
    listen(connection->sockid, 5);
    
    return connection;
}

Connection *connectToServer(char *hostname, int port) {
    Connection *connection = NULL;
    struct hostent *server;

    server = gethostbyname(hostname);

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        return connection;
    }
    connection = calloc(1, sizeof(Connection));
    
    if ((connection->sockid = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        fprintf(stderr, "ERROR opening socket\n");
    
    connection->addr.sin_family = AF_INET;     
    connection->addr.sin_port = htons(port);    
    connection->addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(connection->addr.sin_zero), 8);     
    
    if (connect(connection->sockid,(struct sockaddr *) &connection->addr,sizeof(connection->addr)) < 0) 
        fprintf(stderr, "ERROR connecting\n");
    
    return connection;
}



Connection *acceptConnection(Connection *connection) {
    Connection *client_conn = calloc(1, sizeof(Connection));
    socklen_t clilen;

    clilen = sizeof(struct sockaddr_in);
    if ((client_conn->sockid = accept(connection->sockid, (struct sockaddr *) &(client_conn->addr), &clilen)) == -1) 
        printf("ERROR on accept");
    
    return client_conn;
}

void readFrom(Connection *remote){
    int n;

    bzero(remote->buffer, BUFFER_SIZE);

    /* read from the socket */
    n = read(remote->sockid, remote->buffer, BUFFER_SIZE);
    if (n < 0) 
        printf("ERROR reading from socket");

}

void writeTo(Connection *remote, char *msg, int len){

    int n;
 
    /* write in the socket */ 
    n = write(remote->sockid,msg,len);
    if (n < 0) 
        printf("ERROR writing to socket");

}

void closeConnection(Connection *connection){
    close(connection->sockid);
    free(connection);
}


