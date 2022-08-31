#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data/any.h"
#include "data/queue.h"
#include "data/string.h"
#include "data/stringbuffer.h"
#include "data/stringbufferstruct.h"
#include "gc/gc.h"
#include "methods/methods.h"

static struct D_StringBuffer* _initialize(struct D_StringBuffer* self);

struct Methods* stringBuffer_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))stringBuffer_boolValue;
    methods->m_free = (void (*)(struct Any*))stringBuffer_free;
    methods->m_show = (void (*)(struct Any*, FILE*))stringBuffer_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))stringBuffer_sizeOf;
    methods->m_structSize = stringBuffer_structSize;
    methods->m_streamReadChar = (bool (*)(struct Any*, char*))stringBuffer_readChar;
    methods->m_streamWriteChar = (bool (*)(struct Any*, char))stringBuffer_writeChar;
    return methods;
}

struct BufferSegment* bufferSegment_new(void) {
    struct BufferSegment* segment = (struct BufferSegment*)malloc(sizeof(struct BufferSegment));
    segment->readIndex = 0;
    segment->writeIndex = 0;
    segment->next = NULL;
    return segment;
}

struct D_StringBuffer* stringBuffer_new(void) {
    struct D_StringBuffer* self = (struct D_StringBuffer*)gc_alloc(T_StringBuffer);
    _initialize(self);
    return self;
}

static struct D_StringBuffer* _initialize(struct D_StringBuffer* self) {
    self->count = 0;
    self->readSegment = self->writeSegment = bufferSegment_new();
    self->nSegments++;
    return self;
}

void stringBuffer_free(struct D_StringBuffer* self) {
    struct BufferSegment* segment = self->readSegment;
    while (segment) {
        struct BufferSegment* next = segment->next;
        free(segment);
        segment = next;
    }
    free(self);
}

struct D_String* stringBuffer_asString(struct D_StringBuffer* self) {
    int count;
    char* chars = stringBuffer_readChars(self, &count);
    struct D_String* string = string_new_move(chars, count);
    return string;
}

bool stringBuffer_boolValue(struct D_StringBuffer* self) {
    return self->count > 0;
}

void stringBuffer_clear(struct D_StringBuffer* self) {
    struct BufferSegment* next = self->readSegment->next;
    while (next) {
        struct BufferSegment* next1 = next->next;
        free(next);
        self->nSegments--;
        next = next1;
    }
    self->readSegment->readIndex = 0;
    self->readSegment->writeIndex = 0;
    self->readSegment->next = NULL;
    self->writeSegment = self->readSegment;
    self->count = 0;
}

int stringBuffer_count(struct D_StringBuffer* self) {
    return self->count;
}

// Read all characters from the buffer but do not remove them.
// Returns a heap-allocated string that the caller owns.
char* stringBuffer_readChars(struct D_StringBuffer* self, int* count) {
    *count = self->count;
    char* chars = (char*)malloc(*count + 1);
    int writeIndex = 0;
    struct BufferSegment* segment = self->readSegment;
    int readIndex = segment->readIndex;
    for (int n=0; n<*count; n++) {
        char c = segment->chars[readIndex++];
        chars[writeIndex++] = c;
        if (readIndex == STRINGBUFFER_SEGMENT_SIZE) {
            segment = segment->next;
            readIndex = 0;
        }
    }
    chars[*count] = 0;
    return chars;
}

bool stringBuffer_readChar(struct D_StringBuffer* self, char* c) {
    if (self->count == 0) {
        return false;
    }
    struct BufferSegment* segment = self->readSegment;
    *c = segment->chars[segment->readIndex++];
    if (segment->readIndex == STRINGBUFFER_SEGMENT_SIZE) {
        struct BufferSegment* next = segment->next;
        free(segment);
        self->nSegments--;
        self->readSegment = next;
    }
    self->count--;
    return true;
}

void stringBuffer_show(struct D_StringBuffer* self, FILE* fd) {
    fputs("StringBuffer{\"", fd);
    int count = self->count;
    struct BufferSegment* segment = self->readSegment;
    int readIndex = segment->readIndex;
    for (int n=0; n<count; n++) {
        char c = segment->chars[readIndex++];
        string_escapifyChar(c, fd);
        if (readIndex == STRINGBUFFER_SEGMENT_SIZE) {
            segment = segment->next;
            readIndex = 0;
        }
    }
    fputs("\"}", fd);
}

void stringBuffer_write(struct D_StringBuffer* self, struct D_String* string) {
    stringBuffer_writeChars(self, string_getChars(string));
}

bool stringBuffer_writeChar(struct D_StringBuffer* self, char c) {
    struct BufferSegment* segment = self->writeSegment;
    if (segment == self->readSegment) {
        if (segment->readIndex == segment->writeIndex) {
            segment->readIndex = segment->writeIndex = 0;
        }
    }
    if (segment->writeIndex == STRINGBUFFER_SEGMENT_SIZE) {
        segment->next = bufferSegment_new();
        self->nSegments++;
        segment = segment->next;
        self->writeSegment = segment;
    }
    segment->chars[segment->writeIndex++] = c;
    self->count++;
    return true;
}

bool stringBuffer_writeChars(struct D_StringBuffer* self, char* chars) {
    char c;
    while ((c = *chars)) {
        stringBuffer_writeChar(self, c);
        chars++;
    }
    return true;
}

size_t stringBuffer_sizeOf(struct D_StringBuffer* self) {
    return sizeof(self) + self->nSegments * sizeof(struct BufferSegment);
}

size_t stringBuffer_structSize(void) {
    return sizeof(struct D_StringBuffer);
}
