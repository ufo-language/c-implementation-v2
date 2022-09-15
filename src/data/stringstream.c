#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data/any.h"
#include "data/integer.h"
#include "data/stringstream.h"
#include "memory/gc.h"
#include "methods/methods.h"

struct D_StringStream {
    struct Any obj;
    char* str;
    int strlen;
    int count;  // number of remaining characters
    int pos;
    int col;
    int line;
    int prevChar;
    int prevNewlineCol;
};

struct Methods* stringStream_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))stringStream_boolValue;
    methods->m_free = (void (*)(struct Any*))stringStream_free;
    methods->m_show = (void (*)(struct Any*, FILE*))stringStream_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))stringStream_sizeOf;
    methods->m_structSize = stringStream_structSize;
    return methods;
}

struct D_StringStream* stringStream_new(char* str) {
    struct D_StringStream* self = (struct D_StringStream*)gc_alloc(T_StringStream);
    self->strlen = self->count = strlen(str);
    self->str = (char*)malloc(self->count + 1);
    strcpy(self->str, str);
    self->pos = -1;
    self->col = -1;
    self->line = 1;
    self->prevChar = -1;
    self->prevNewlineCol = -1;
    return self;
}

void stringStream_free(struct D_StringStream* self) {
    free(self->str);
    free(self);
}

bool stringStream_boolValue(struct D_StringStream* self) {
    return self->count > 0;
}

int stringStream_count(struct D_StringStream* self) {
    return self->count;
}

int stringStream_get(struct D_StringStream* self) {
    self->pos++;
    self->col++;
    char c = self->str[self->pos];
    if (c == '\n') {
        self->prevNewlineCol = self->col;
    }
    if (self->prevChar == '\n') {
        self->col = 0;
        self->line++;
    }
    self->prevChar = c;
    return c;
}

// This returns the position of the last character read with get().
void stringStream_getPos(struct D_StringStream* self, int* pos, int* col, int* line) {
    *pos = self->pos;
    *col = self->col;
    *line = self->line;
}

void stringStream_show(struct D_StringStream* self, FILE* fp) {
    fputs("StringStream{\"", fp);
    for (int n=0; n<self->strlen; n++ ) {
        if (n == self->pos) {
            fputs("|>", fp);
        }
        fputc(self->str[n], fp);
    }
    fputs("\"}", fp);
}

void stringStream_unget(struct D_StringStream* self) {
    self->pos--;
    self->col--;
    if (self->col < 0) {
        self->line--;
        self->col = self->prevNewlineCol;
    }
}

size_t stringStream_sizeOf(struct D_StringStream* self) {
    return sizeof(self);
}

size_t stringStream_structSize(void) {
    return sizeof(struct D_StringStream);
}
