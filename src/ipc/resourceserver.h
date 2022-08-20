#if !defined(RESOURCESERVER_H)
#define RESOURCESERVER_H

#include <sys/types.h>

void resourceServer(pid_t childPid, int readFromUFO, int writeToUFO);

#endif
