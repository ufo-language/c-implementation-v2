#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/file.h"
#include "data/integer.h"
#include "data/string.h"
#include "data/stringbuffer.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "main/globals.h"
#include "memory/gc.h"
#include "methods/methods.h"

struct D_File {
    struct Any obj;
    struct D_String* fileName;
    FILE* fp;
    bool isOpen;
};

bool file_writeChar(struct D_File* self, char c);

struct Methods* file_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))file_boolValue;
    methods->m_free = (void (*)(struct Any*))file_free;
    methods->m_markChildren = (void (*)(struct Any* self))file_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))file_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))file_sizeOf;
    methods->m_streamReadChar = (bool (*)(struct Any*, char*))file_readChar;
    methods->m_structSize = file_structSize;
    methods->m_streamWriteChar = (bool (*)(struct Any* self, char c))file_writeChar;
    return methods;
}

struct D_File* file_fromFilePointer(struct D_String* fileName, FILE* fp) {
    struct D_File* self = (struct D_File*)gc_alloc(T_File);
    self->fileName = fileName;
    self->fp = fp;
    return self;
}

struct D_File* file_new(struct D_String* fileName) {
    struct D_File* self = (struct D_File*)gc_alloc(T_File);
    self->fileName = fileName;
    self->fp = NULL;
    return self;
}

struct D_File* file_new_charStringName(char* fileName) {
    struct D_String* fileNameString = string_new(fileName);
    return file_new(fileNameString);
}

void file_free(struct D_File* self) {
    free(self);
}

bool file_boolValue(struct D_File* self) {
    return file_isOpen(self);
}

void file_close(struct D_File* self, struct Evaluator* etor) {
    if (fclose(self->fp) != 0) {
        evaluator_throwException(
            etor,
            any_typeSymbol((struct Any*)self),
            " unable to close file",
            (struct Any*)self->fileName
        );
    }
    else {
        self->fp = NULL;
    }
}

bool file_isOpen(struct D_File* self) {
    return self->fp == NULL ? false : true;
}

void file_markChildren(struct D_File* self) {
    any_mark((struct Any*)self->fileName);
}

int file_open(struct D_File* self) {
    char* fileName = string_getChars(self->fileName);
    FILE* fp = fopen(fileName, "r");
    if (fp == NULL) {
        return errno;
    }
    self->fp = fp;
    return 0;
}

/*
bool file_open_aux(struct D_File* self) {
    char* fileName = string_getChars(self->fileName);
    self->fp = fopen(fileName, "r");
    return self->fp != NULL;
}
*/

bool file_readChar(struct D_File* self, char* c) {
    int c1 = fgetc(self->fp);
    if (c1 == EOF) {
        return false;
    }
    *c = c1;
    return true;
}

size_t file_readAll(struct D_File* self, struct D_StringBuffer* stringBuffer, struct Evaluator* etor) {
    size_t nBytesRead = file_readAll_stringBuffer(self, stringBuffer, etor);
    return nBytesRead;
}

size_t file_readAll_stringBuffer(struct D_File* self, struct D_StringBuffer* stringBuffer, struct Evaluator* etor) {
    FILE* fp = self->fp;
    fseek(fp, 0L, SEEK_END);
    long fileSize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        int n1 = (int)(fileSize / INT_MAX);
        int n2 = (int)(fileSize % INT_MAX);
        struct D_Array* sizeAry = array_newN(2, integer_new(n1), integer_new(n2));
        evaluator_throwException(
            etor,
            any_typeSymbol((struct Any*)self),
            "unable to allocate buffer",
            (struct Any*)array_newN(2, self->fileName, (struct Any*)sizeAry)
        );
    }
    size_t nBytesRead = fread(buffer, sizeof(char), fileSize, fp);
    buffer[fileSize] = 0;
    stringBuffer_writeChars(stringBuffer, buffer);
    free(buffer);
    return nBytesRead;
}

void file_show(struct D_File* self, FILE* fp) {
    fprintf(fp, "File{name=\"%s\", isOpen=", string_getChars(self->fileName));
    any_show((struct Any*)(self->fp == NULL ? FALSE : TRUE), fp);
    fputc('}', fp);
}

long file_size(struct D_File* self) {
    return ftell(self->fp);
}

size_t file_sizeOf(struct D_File* self) {
    return sizeof(self);
}

size_t file_structSize(void) {
    return sizeof(struct D_File);
}

bool file_writeChar(struct D_File* self, char c) {
    fputc(c, self->fp);
    return true;
}
