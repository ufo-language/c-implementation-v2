#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
//#include <unistd.h>  // for write()

#include <arpa/inet.h>  // for inet_addr(), htons()
#include <netinet/in.h>  // for struct sockaddr_in
#include <sys/socket.h>  // for socket(), connect()

#include "data/any.h"
#include "data/boolean.h"
#include "data/string.h"
#include "data/tcpclient.h"
#include "etor/evaluator.h"
#include "io/tcpclient.h"
#include "main/globals.h"
#include "memory/gc.h"
#include "methods/methods.h"

struct D_TCPClient {
    struct Any obj;
    struct D_String* remoteAddressString;
    struct sockaddr_in remoteAddress;
    int port;
    int sockfd;
    bool isOpen;
};

struct Methods* tcpClient_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))tcpClient_boolValue;
    methods->m_free = (void (*)(struct Any*))tcpClient_free;
    methods->m_markChildren = (void (*)(struct Any* self))tcpClient_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))tcpClient_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))tcpClient_sizeOf;
    //methods->m_streamReadChar = (bool (*)(struct Any*, char*))tcpClient_readChar;
    methods->m_structSize = tcpClient_structSize;
    methods->m_streamWriteChar = (bool (*)(struct Any* self, char c))tcpClient_writeChar;
    return methods;
}

// Examples found here https://jameshfisher.com/2017/04/05/set_socket_nonblocking/

int guard(int n, char * err) { if (n == -1) { perror(err); exit(1); } return n; }

// working
struct D_TCPClient* tcpClient_new(struct D_String* remoteAddressString, int port) {
    struct D_TCPClient* self = (struct D_TCPClient*)gc_alloc(T_TCPClient);
    self->remoteAddressString = remoteAddressString;
    self->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    self->remoteAddress.sin_family = AF_INET;
    self->remoteAddress.sin_port = htons(port);
    self->isOpen = false;
    // set the socket to be non-blocking
    int flags = guard(fcntl(self->sockfd, F_GETFL), "could not get socket flags");
    guard(fcntl(self->sockfd, F_SETFL, flags | O_NONBLOCK), "could not set socket to be non-blocking");
    return self;
}

void tcpClient_free(struct D_TCPClient* self) {
    free(self);
}

bool tcpClient_boolValue(struct D_TCPClient* self) {
    return tcpClient_isOpen(self);
}

int tcpClient_close(struct D_TCPClient* self) {
    self->isOpen = false;
    if (close(self->sockfd)) {
        return errno;
    }
    return 0;
}

// working
void tcpClient_connect(struct D_TCPClient* self, struct Evaluator* etor) {
    char* addressChars = string_getChars(self->remoteAddressString);
    self->remoteAddress.sin_addr.s_addr = inet_addr(addressChars);
    self->isOpen = false;
    io_tcpClient_open(etor, self);
    self->isOpen = true;
}

struct sockaddr_in* tcpClient_getRemoteAddress(struct D_TCPClient* self) {
    return &self->remoteAddress;
}

int tcpClient_getSockFd(struct D_TCPClient* self) {
    return self->sockfd;
}

bool tcpClient_isOpen(struct D_TCPClient* self) {
    return self->isOpen;
}

void tcpClient_markChildren(struct D_TCPClient* self) {
    any_mark((struct Any*)self->remoteAddressString);
}

bool tcpClient_readChar(struct D_TCPClient* self, char* c) {
    (void)self;
    (void)c;
    /*
    int c1 = fgetc(self->fp);
    if (c1 == EOF) {
        return false;
    }
    *c = c1;
    */
    return false;
}

void tcpClient_show(struct D_TCPClient* self, FILE* fp) {
    fputs("TCPClient{address=", fp);
    string_show(self->remoteAddressString, fp);
    fputs(", ", fp);
    fprintf(fp, "port=%d, open=", self->port);
    boolean_show(boolean_from(self->isOpen), fp);
    fputc('}', fp);
}

size_t tcpClient_sizeOf(struct D_TCPClient* self) {
    return sizeof(self);
}

size_t tcpClient_structSize(void) {
    return sizeof(struct D_TCPClient);
}

bool tcpClient_writeChar(struct D_TCPClient* self, char c) {
    if (!self->isOpen) {
        return false;
    }
    return write(self->sockfd, &c, 1) == 1 ? true : false;
}

#if 0
// working
int tcpClient_writeString(struct D_TCPClient* self, struct D_String* string) {
    if (!self->isOpen) {
        return -1;
    }
    char* chars = string_getChars(string);
    int count = string_count(string);
    return (int)write(self->sockfd, chars, (size_t)count);
}
#else
void tcpClient_writeString(struct D_TCPClient* self, struct D_String* string, struct Evaluator* etor) {
    io_tcpClient_writeString(etor, self, string);
}

#endif
