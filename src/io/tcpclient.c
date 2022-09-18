#include <errno.h>
#include <string.h>  // for strerror
#include <sys/socket.h>  // for connect()
#include <unistd.h>  // for write()

#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "data/string.h"
#include "data/stringbuffer.h"
#include "data/stringbufferstruct.h"
#include "data/tcpclient.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "io/tcpclient.h"
#include "main/globals.h"

static void _open_contin(struct E_Continuation* contin, struct Evaluator* etor) {
    struct D_TCPClient* tcpClient = (struct D_TCPClient*)continuation_getArg(contin);
    int res = connect(tcpClient_getSockFd(tcpClient), (struct sockaddr*)tcpClient_getRemoteAddress(tcpClient), sizeof(struct sockaddr));
    if (res == 0) {
        evaluator_pushObj(etor, (struct Any*)NIL);
        return;
    }
    int errorNumber = errno;
    char* errorString = strerror(errorNumber);
    switch (errno) {
        case EAGAIN:
        case EINPROGRESS:
            evaluator_setThreadStatus(etor, TS_Yield);
            evaluator_pushExpr(etor, (struct Any*)contin);
            break;
        case ECONNREFUSED:
            evaluator_throwException(etor, any_typeSymbol((struct Any*)tcpClient), errorString, (struct Any*)tcpClient);
            break;
        default:
            evaluator_throwException(etor,
                                     any_typeSymbol((struct Any*)tcpClient),
                                     "tcpclient.c case not handled",
                                     (struct Any*)array_newN(2, integer_new(errorNumber), string_new(errorString))
            );
    }
}

void io_tcpClient_open(struct Evaluator* etor, struct D_TCPClient* tcpClient) {
    struct E_Continuation * contin = continuation_new(_open_contin, "tcpClient_open", (struct Any*)tcpClient);
    _open_contin(contin, etor);
}

static void _readString_contin(struct E_Continuation* contin, struct Evaluator* etor) {
    struct D_Array* argAry = (struct D_Array*)continuation_getArg(contin);
    struct D_TCPClient* tcpClient = (struct D_TCPClient*)array_get_unsafe(argAry, 0);
    int sockfd = tcpClient_getSockFd(tcpClient);
    struct D_StringBuffer* inputBuffer = (struct D_StringBuffer*)array_get_unsafe(argAry, 1);
    char buffer[STRINGBUFFER_SEGMENT_SIZE];
    ssize_t res = read(sockfd, buffer, STRINGBUFFER_SEGMENT_SIZE);
    if (res >= 0) {
        stringBuffer_writeChars(inputBuffer, buffer);
        if (res == STRINGBUFFER_SEGMENT_SIZE) {
            // read again
            evaluator_setThreadStatus(etor, TS_Yield);
            evaluator_pushExpr(etor, (struct Any*)contin);
        }
        else {
            evaluator_pushObj(etor, (struct Any*)stringBuffer_asString(inputBuffer));
        }
        return;
    }
    int errorNumber = errno;
    char* errorString = strerror(errorNumber);
    switch (errno) {
        case EWOULDBLOCK:
            // return the string
            evaluator_pushObj(etor, (struct Any*)stringBuffer_asString(inputBuffer));
            break;
        default:
            evaluator_throwException(etor,
                                     any_typeSymbol((struct Any*)tcpClient),
                                     "tcpclient.c case not handled",
                                     (struct Any*)array_newN(2, integer_new(errorNumber), string_new(errorString))
            );
    }
}

void io_tcpClient_readString(struct Evaluator* etor, struct D_TCPClient* tcpClient) {
    struct D_StringBuffer* inputBuffer = stringBuffer_new();
    struct D_Array* argAry = array_newN(2, tcpClient, inputBuffer);
    struct E_Continuation * contin = continuation_new(_readString_contin, "tcpClient_readString", (struct Any*)argAry);
    _readString_contin(contin, etor);
}

static void _writeString_contin(struct E_Continuation* contin, struct Evaluator* etor) {
    struct D_Array* argAry = (struct D_Array*)continuation_getArg(contin);
    struct D_TCPClient* tcpClient = (struct D_TCPClient*)array_get_unsafe(argAry, 0);
    int sockfd = tcpClient_getSockFd(tcpClient);
    struct D_String* string = (struct D_String*)array_get_unsafe(argAry, 1);
    char* chars = string_getChars(string);
    size_t count = (size_t)string_count(string);
    ssize_t res = write(sockfd, chars, count);
    if (res != -1) {
        evaluator_pushObj(etor, (struct Any*)NIL);
        return;
    }
    int errorNumber = errno;
    char* errorString = strerror(errorNumber);
    switch (errno) {
#if 0
        // I don't know what error values are likely to be encountered
        case EAGAIN:
        case EINPROGRESS:
            evaluator_setThreadStatus(etor, TS_Yield);
            evaluator_pushExpr(etor, (struct Any*)contin);
            break;
        case ECONNREFUSED:
            evaluator_throwException(etor, any_typeSymbol((struct Any*)tcpClient), errorString, (struct Any*)tcpClient);
            break;
#endif
        default:
            evaluator_throwException(etor,
                                     any_typeSymbol((struct Any*)tcpClient),
                                     "tcpclient.c case not handled",
                                     (struct Any*)array_newN(2, integer_new(errorNumber), string_new(errorString))
            );
    }
}

void io_tcpClient_writeString(struct Evaluator* etor, struct D_TCPClient* tcpClient, struct D_String* string) {
    struct D_Array* arg = array_newN(2, (struct Any*)tcpClient, (struct Any*)string);
    struct E_Continuation * contin = continuation_new(_writeString_contin, "tcpClient_writeString", (struct Any*)arg);
    _writeString_contin(contin, etor);
}
