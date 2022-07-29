#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "data/stringbufferstruct.h"
#include "data/stringbuffer.h"
#include "main/ipc.h"

void _handleDisplayString(int readFromUfo);
void _handleUfoCommand(int readFromUfo, int writeToUfo);

static bool _continue;

void resourceServer(pid_t ufoPid, int readFromUfo, int writeToUfo) {
    _continue = true;
    while (_continue) {
        char msgType = ipc_readChar(readFromUfo);
        switch (msgType) {
            case 'D':
                _handleDisplayString(readFromUfo);
                break;
            case 'U':
                _handleUfoCommand(readFromUfo, writeToUfo);
                break;
        }
    }
    int childStatus;
    waitpid(ufoPid, &childStatus, 0);  // wait for UFO to terminate
}

void _handleDisplayString(int readFromUfo) {
    char c;
    while ((c = ipc_readChar(readFromUfo))) {
        putchar(c);
    }
}

void _handleReadString(int writeToUfo) {
    static struct D_StringBuffer sb;
    while (true) {
        // read text from the user
        (void)writeToUfo;
    }
    int nChars = stringBuffer_count(&sb);
    char c;
    for (int n=0; n<nChars; n++) {
        if (stringBuffer_readChar(&sb, &c)) {
            // -> send to UFO
        }
        else {
            // string buffer is empty, but the counted for loop ensures that we never get here
        }
    }
}

void _handleUfoCommand(int readFromUfo, int writeToUfo) {
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
