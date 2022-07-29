#include "data/stringbuffer.h"

void io_readLine(struct D_StringBuffer* sb) {
    char c;
    while ((c = getchar()) != '\n') {
        stringBuffer_writeChar(sb, c);
    }
}
