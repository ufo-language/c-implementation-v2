#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "data/stringbufferstruct.h"
#include "data/string.h"
#include "data/stringbuffer.h"
#include "ipc/ipc.h"

static void _handleDisplayString(int readFromUfo);
static void _handleReadLine(int writeToUfo);
static void _handleUfoCommand(int readFromUfo, int writeToUfo);

static bool _continue;

void resourceServer(pid_t ufoPid, int readFromUfo, int writeToUfo) {
    _continue = true;
    while (_continue) {
        char msgType = ipc_readChar(readFromUfo);
        switch (msgType) {
            case 'D':
                _handleDisplayString(readFromUfo);
                break;
            case 'R':
                printf("%s got R\n", __func__);
                _handleReadLine(writeToUfo);
                break;
            case 'U':
                _handleUfoCommand(readFromUfo, writeToUfo);
                break;
        }
    }
    int childStatus;
    waitpid(ufoPid, &childStatus, 0);  // wait for UFO to terminate
}

static void _handleDisplayString(int readFromUfo) {
    char c;
    while ((c = ipc_readChar(readFromUfo))) {
        putchar(c);
    }
}

static void _handleReadLine(int writeToUfo) {
    static struct D_StringBuffer* sb;
    printf("%s called\n", __func__);
    while (true) {
        int chr = fgetc(stdin);
        printf("  got char %d\n", chr);
        if (chr == -1) {
            break;
        }
        if (chr == '\n') {
            break;
        }
        stringBuffer_writeChar(sb, chr);
    }
    int nChars = stringBuffer_count(sb);
    printf("%s got %d chars\n", __func__, nChars);
    struct D_String* string = stringBuffer_asString(sb);
    char* chars = string_getChars(string);
    printf("%s got string '%s'\n", __func__, chars);
    ipc_writeString(writeToUfo, nChars, chars);
}

static void _handleUfoCommand(int readFromUfo, int writeToUfo) {
    (void)writeToUfo;
    char cmd = ipc_readChar(readFromUfo);
    switch (cmd) {
        case 'Q':
            _continue = false;
            break;
        default:
            fprintf(stderr, "%s got unknown subcommand '%c'\n", __func__, cmd);
    }
}
