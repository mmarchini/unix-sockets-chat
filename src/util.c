#include <string.h>
#include <netdb.h>
#include <time.h>
#include "util.h"

time_t timestamp() {
    return time(NULL);
}

void time_to_string(char *dest, int len, time_t timestamp){
	bzero(dest, len);
	strftime(dest, len, "%d/%m/%Y %H:%M", localtime(&timestamp));
}
