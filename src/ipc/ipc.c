#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int PARENT_TO_CHILD[2];
int CHILD_TO_PARENT[2];

char ipc_readChar(int fd) {
    char c;
    // this reads the char from the resource server process pipe
    //printf("%s waiting to read a char\n", __func__);
    ssize_t nCharsRead = read(fd, &c, sizeof(char));
    //printf("%s read %d chars = %d\n", __func__, (int)nCharsRead, c);
    if (nCharsRead != sizeof(char)) {
        fprintf(stderr, "%s read %ld of %lu bytes\n", __func__, (unsigned long)nCharsRead, sizeof(char));
    }
    return c;
}

void ipc_writeChar(int fd, char c) {
    // this sends the char through the pipe to the resource server process
    ssize_t nCharsWritten = write(fd, &c, sizeof(char));
    if (nCharsWritten != sizeof(char)) {
        fprintf(stderr, "%s wrote %ld of %lu bytes\n", __func__, (unsigned long)nCharsWritten, sizeof(char));
    }
}

int ipc_readInt(int fd) {
    int n;
    // this reads the int from the resource server process pipe
    ssize_t nCharsRead = read(fd, &n, sizeof(int));
    if (nCharsRead != sizeof(int)) {
        fprintf(stderr, "%s read %ld of %lu bytes\n", __func__, (unsigned long)nCharsRead, sizeof(int));
    }
    return n;
}

void ipc_writeInt(int fd, int n) {
    // this sends the int through the pipe to the resource server process
    ssize_t nCharsWritten = write(fd, &n, sizeof(int));
    if (nCharsWritten != sizeof(int)) {
        fprintf(stderr, "%s wrote %ld of %lu bytes\n", __func__, (unsigned long)nCharsWritten, sizeof(int));
    }
}

long ipc_readLong(int fd) {
    long n;
    // this reads the long from the resource server process pipe
    ssize_t nCharsRead = read(fd, &n, sizeof(long));
    if (nCharsRead != sizeof(long)) {
        fprintf(stderr, "%s read %ld of %lu bytes\n", __func__, (unsigned long)nCharsRead, sizeof(long));
    }
    return n;
}

void ipc_writeLong(int fd, long n) {
    // this sends the long through the pipe to the resource server process
    ssize_t nCharsWritten = write(fd, &n, sizeof(long));
    if (nCharsWritten != sizeof(long)) {
        fprintf(stderr, "%s wrote %ld of %lu bytes\n", __func__, (unsigned long)nCharsWritten, sizeof(long));
    }
}

size_t ipc_readSizeT(int fd) {
    size_t n;
    // this reads the number from the resource server process pipe
    ssize_t nCharsRead = read(fd, &n, sizeof(size_t));
    if (nCharsRead != sizeof(long)) {
        fprintf(stderr, "%s read %ld of %lu bytes\n", __func__, (unsigned long)nCharsRead, sizeof(size_t));
    }
    return n;
}

void ipc_writeSizeT(int fd, size_t n) {
    // this sends the number through the pipe to the resource server process
    ssize_t nCharsWritten = write(fd, &n, sizeof(size_t));
    if (nCharsWritten != sizeof(size_t)) {
        fprintf(stderr, "%s wrote %ld of %lu bytes\n", __func__, (unsigned long)nCharsWritten, sizeof(size_t));
    }
}

void ipc_readString(int fd, ssize_t nChars, char* buffer) {
    // this reads the string from the resource server process pipe
    ssize_t nCharsRead = read(fd, buffer, nChars);
    if (nCharsRead != nChars) {
        fprintf(stderr, "%s read %ld of %lu bytes\n", __func__, (unsigned long)nCharsRead, nChars);
    }
}

void ipc_writeString(int fd, ssize_t nChars, char* buffer) {
    // this sends the string through the pipe to the resource server process
    ssize_t nCharsWritten = write(fd, buffer, nChars);
    if (nCharsWritten != nChars) {
        fprintf(stderr, "%s wrote %ld of %lu bytes\n", __func__, nCharsWritten, nChars);
    }
}

// WARNING: the return value is heap-allocated and the receiver owns it
// after calling this function
char* ipc_receiveMessage(int fd, size_t* msgLen) {
    *msgLen = ipc_readSizeT(fd);
    char* msg = (char*)malloc(*msgLen * sizeof(char) + 1);
    ipc_readString(fd, *msgLen, msg);
    msg[*msgLen] = 0;
    return msg;
}

void ipc_sendMessage(int fd, size_t msgLen, char* msgBuf) {
    ipc_writeSizeT(fd, msgLen);
    ipc_writeString(fd, msgLen, msgBuf);
}
