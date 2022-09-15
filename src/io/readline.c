#include <poll.h>
#include <stdbool.h>
#include <unistd.h>  // for read()

#include "data/any.h"
#include "data/stringbuffer.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"

static bool _fdCanRead(int fd) {
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    if (poll(&pfd, 1, 0)) {
        if (pfd.revents & POLLIN) {
            return true;
        }
    }
    return false;
}

enum ReadResult {
    RR_Blocked,
    RR_Sentinel,
    RR_EOF
};

static enum ReadResult _doRead(struct D_StringBuffer* sb, int sentinel, bool append) {
    int stdinFd = stdin->_fileno;
    while (true) {
        if (!_fdCanRead(stdinFd)) {
            return RR_Blocked;
        }
        int chr = 0;
        size_t nCharsRead = read(stdinFd, &chr, 1);
        if (nCharsRead == 0) {
            return RR_EOF;
        }
        if (chr == sentinel) {
            if (append) {
                stringBuffer_writeChar(sb, chr);
            }
            return RR_Sentinel;
        }
        stringBuffer_writeChar(sb, chr);
    }
}

static void _readLine_contin(struct E_Continuation* contin, struct Evaluator* etor) {
    struct D_StringBuffer* sb = (struct D_StringBuffer*)continuation_getArg(contin);
    switch (_doRead(sb, '\n', false)) {
        case RR_Blocked:
            evaluator_pushExpr(etor, (struct Any*)contin);
            break;
        case RR_EOF:
        case RR_Sentinel:
            evaluator_pushExpr(etor, (struct Any*)stringBuffer_asString(sb));
            break;
    }
}

void io_readLine_nonBlocking(struct Evaluator* etor, struct D_StringBuffer* sb) {
    struct E_Continuation* contin = continuation_new(_readLine_contin, "readLine", (struct Any*)sb);
    _readLine_contin(contin, etor);
}
