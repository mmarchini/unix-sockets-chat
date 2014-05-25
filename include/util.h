#ifndef _UTIL_H
#define _UTIL_H

#include <time.h>

// Macros to lock/unlock mutex
// This gives the impresson that a block of code between BEGIN/END is atomic.
#define CRITICAL_REGION_BEGIN(mutex) { pthread_mutex_lock(&(mutex));
#define CRITICAL_REGION_END(mutex) pthread_mutex_unlock(&(mutex)); }

typedef enum {
	True = 1,
	False = 0
} bool;

time_t timestamp(); 

void time_to_string(char *dest, int len, time_t timestamp);

#endif
