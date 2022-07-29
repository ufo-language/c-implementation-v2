#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char ipc_readChar(int fd) {
    char c;
    ssize_t nCharsRead = read(fd, &c, sizeof(char));
    if (nCharsRead != sizeof(char)) {
        fprintf(stderr, "%s read %ld of %lu bytes\n", __func__, nCharsRead, sizeof(char));
    }
    return c;
}

void ipc_writeChar(int fd, char c) {
    ssize_t nCharsWritten = write(fd, &c, sizeof(char));
    if (nCharsWritten != sizeof(char)) {
        fprintf(stderr, "%s wrote %ld of %lu bytes\n", __func__, nCharsWritten, sizeof(char));
    }
}

int ipc_readInt(int fd) {
    int n;
    ssize_t nCharsRead = read(fd, &n, sizeof(int));
    if (nCharsRead != sizeof(int)) {
        fprintf(stderr, "%s read %ld of %lu bytes\n", __func__, nCharsRead, sizeof(int));
    }
    return n;
}

void ipc_writeInt(int fd, int n) {
    ssize_t nCharsWritten = write(fd, &n, sizeof(int));
    if (nCharsWritten != sizeof(int)) {
        fprintf(stderr, "%s wrote %ld of %lu bytes\n", __func__, nCharsWritten, sizeof(int));
    }
}

long ipc_readLong(int fd) {
    long n;
    ssize_t nCharsRead = read(fd, &n, sizeof(long));
    if (nCharsRead != sizeof(long)) {
        fprintf(stderr, "%s read %ld of %lu bytes\n", __func__, nCharsRead, sizeof(long));
    }
    return n;
}

void ipc_writeLong(int fd, long n) {
    ssize_t nCharsWritten = write(fd, &n, sizeof(long));
    if (nCharsWritten != sizeof(long)) {
        fprintf(stderr, "%s wrote %ld of %lu bytes\n", __func__, nCharsWritten, sizeof(long));
    }
}

size_t ipc_readSizeT(int fd) {
    size_t n;
    ssize_t nCharsRead = read(fd, &n, sizeof(size_t));
    if (nCharsRead != sizeof(long)) {
        fprintf(stderr, "%s read %ld of %lu bytes\n", __func__, nCharsRead, sizeof(size_t));
    }
    return n;
}

void ipc_writeSizeT(int fd, size_t n) {
    ssize_t nCharsWritten = write(fd, &n, sizeof(size_t));
    if (nCharsWritten != sizeof(size_t)) {
        fprintf(stderr, "%s wrote %ld of %lu bytes\n", __func__, nCharsWritten, sizeof(size_t));
    }
}

void ipc_readString(int fd, ssize_t nChars, char* buffer) {
    ssize_t nCharsRead = read(fd, buffer, nChars);
    if (nCharsRead != nChars) {
        fprintf(stderr, "%s read %ld of %lu bytes\n", __func__, nCharsRead, nChars);
    }
}

void ipc_writeString(int fd, ssize_t nChars, char* buffer) {
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
