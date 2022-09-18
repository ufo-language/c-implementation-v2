#include <errno.h>
#include <string.h>  // for strerror
#include <sys/socket.h>  // for connect()
#include <unistd.h>  // for write()

#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "data/string.h"
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

static void _writeString_contin(struct E_Continuation* contin, struct Evaluator* etor) {
    struct D_Array* argAry = (struct D_Array*)continuation_getArg(contin);
    struct D_TCPClient* tcpClient = (struct D_TCPClient*)array_get_unsafe(argAry, 0);
    int sockfd = tcpClient_getSockFd(tcpClient);
    struct D_String* string = (struct D_String*)array_get_unsafe(argAry, 1);
    char* chars = string_getChars(string);
    size_t count = string_count(string);
    int res = write(sockfd, chars, (size_t)count);
    if (res != -1) {
        evaluator_pushObj(etor, (struct Any*)NIL);
        return;
    }
    int errorNumber = errno;
    char* errorString = strerror(errorNumber);
    switch (errno) {
#if 0
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

void io_tcpClient_writeString(struct Evaluator* etor, struct D_TCPClient* self, struct D_String* string) {
    struct D_Array* arg = array_newN(2, (struct Any*)self, (struct Any*)string);
    struct E_Continuation * contin = continuation_new(_writeString_contin, "tcpClient_writeString", (struct Any*)arg);
    _writeString_contin(contin, etor);
}
