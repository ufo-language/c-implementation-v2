#if !defined(IPC_H)
#define IPC_H

#include <sys/types.h>

// these are initialized in main.c
extern int PARENT_TO_CHILD[2];
extern int CHILD_TO_PARENT[2];

char ipc_readChar(int fd);
int ipc_readInt(int fd);
long ipc_readLong(int fd);
void ipc_readString(int fd, ssize_t nChars, char* buffer);

void ipc_writeChar(int fd, char c);
void ipc_writeInt(int fd, int n);
void ipc_writeLong(int fd, long n);
void ipc_writeString(int fd, ssize_t nChars, char* buffer);

char* ipc_receiveMessage(int fd, size_t* msgLen);
void ipc_sendMessage(int fd, int msgLen, char* msgBuf);

#endif
