#include <stdlib.h>

#define PORT 4000

int main(int argc, char *argv[]) {
    int port;

    if(argc < 2)
        port = PORT;
    else if(argc == 2)
        port = atoi(argv[1]);
    else
        return 1;

    return startServer(port);
}
