#include <stdlib.h>
#include "client.h"

int main(int argc, char *argv[]) {
    int port;

    if(argc < 4)
        return 1;

	port = atoi(argv[2]);

    return startClient(argv[1], port, argv[3]);
}
