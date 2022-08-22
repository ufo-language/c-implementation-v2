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
#include "dispatch/methodtable.h"
#include "etor/evaluator.h"
#include "gc/gc.h"
#include "main/globals.h"

struct D_File {
    struct Any obj;
    struct D_String* fileName;
    FILE* fp;
    bool isOpen;
};

struct Methods* file_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))file_boolValue;
    methods->m_free = (void (*)(struct Any*))file_free;
    methods->m_markChildren = (void (*)(struct Any* self))file_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))file_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))file_sizeOf;
    methods->m_structSize = array_structSize;
    return methods;
}

struct D_File* file_new(struct D_String* fileName) {
    struct D_File* self = (struct D_File*)gc_alloc(T_File);
    self->fileName = fileName;
    self->fp = NULL;
    return self;
}

struct D_File* file_new_charString(char* fileName) {
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

void file_open(struct D_File* self, struct Evaluator* etor) {
    char* fileName = string_getChars(self->fileName);
    FILE* fp = fopen(fileName, "r");
    if (fp == NULL) {
        evaluator_throwException(
            etor,
            any_typeSymbol((struct Any*)self),
            "unable to open file",
            (struct Any*)self->fileName
        );
    }
    self->fp = fp;
}

bool file_open_aux(struct D_File* self) {
    char* fileName = string_getChars(self->fileName);
    self->fp = fopen(fileName, "r");
    return self->fp != NULL;
}

char file_readChar(struct D_File* self) {
    return fgetc(self->fp);
}

// This returns the number of bytes read, but since the number is an int
// and the number of bytes read can be an unsigned long, the integer is
// split into two parts returned as an array.
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
