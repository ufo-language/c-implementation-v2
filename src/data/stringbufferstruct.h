#if !defined(STRINGBUFFERSTRUCT_H)
#define STRINGBUFFERSTRUCT_H

#include "any.h"

#define STRINGBUFFER_SEGMENT_SIZE 128

struct BufferSegment {
    char chars [STRINGBUFFER_SEGMENT_SIZE];
    int readIndex;
    int writeIndex;
    struct BufferSegment* next;
};

struct D_StringBuffer {
    struct Any obj;
    int count;
    int nSegments;
    struct BufferSegment* readSegment;
    struct BufferSegment* writeSegment;
};

#endif
