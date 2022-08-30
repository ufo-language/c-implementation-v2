#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/file.h"
#include "data/stream.h"
#include "data/string.h"
#include "data/stringbuffer.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "methods/methods.h"

struct D_Stream {
    struct Any obj;
    struct Any* substream;
};

struct Methods* stream_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))stream_boolValue;
    methods->m_free = (void (*)(struct Any*))stream_free;
    methods->m_markChildren = (void (*)(struct Any* self))stream_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))stream_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))stream_sizeOf;
    methods->m_streamReadChar = (bool (*)(struct Any*, char*))stream_readChar;
    methods->m_streamWriteChar = (bool (*)(struct Any*, char))stream_writeChar;
    methods->m_structSize = stream_structSize;
    return methods;
}

struct D_Stream* stream_new(struct D_Symbol* typeSym, struct Any* obj, struct Evaluator* etor) {
    if (typeSym == SYM_STRING) {
        enum TypeId typeId = any_typeId(obj);
        if (typeId == T_String) {
            struct D_StringBuffer* buffer = stringBuffer_new();
            stringBuffer_write(buffer, (struct D_String*)obj);
            struct D_Stream* stream = stream_new_aux((struct Any*)buffer);
            return stream;
        }
        struct D_Array* arg = array_newN(2, typeSym, obj);
        evaluator_throwException(etor,
                                 symbol_new("Stream"),
                                 "illegal stream object for stream type",
                                 (struct Any*)arg);
    }
    else if (typeSym == SYM_INFILE) {
        enum TypeId typeId = any_typeId(obj);
        if (typeId == T_String) {
            struct D_String* fileNameStr = (struct D_String*)obj;
            struct D_File* file = file_new(fileNameStr);
            struct D_Stream* stream = stream_new_aux((struct Any*)file);
            return stream;
        }
        struct D_Array* arg = array_newN(2, typeSym, obj);
        evaluator_throwException(etor,
                                 symbol_new("Stream"),
                                 "illegal stream object for stream type",
                                 (struct Any*)arg);
    }
    else if (typeSym == SYM_OUTFILE) {
        printf("%s OutFile\n", __func__);
        // TODO finish
    }
    evaluator_throwException(etor,
                             symbol_new("Stream"),
                             "illegal stream type",
                             (struct Any*)typeSym);
    return NULL;
}

struct D_Stream* stream_new_aux(struct Any* substream) {
    struct D_Stream* self = (struct D_Stream*)gc_alloc(T_Stream);
    self->substream = substream;
    return self;
}

void stream_free(struct D_Stream* self) {
    free(self);
}

bool stream_boolValue(struct D_Stream* self) {
    return METHOD_TABLE[self->substream->typeId]->m_boolValue(self->substream);
}

bool stream_hasNext(struct D_Stream* self) {
    //return METHOD_TABLE[self->streamTypeId]->m_streamHasNext(self);
    (void)self;
    return false;
}

void stream_markChildren(struct D_Stream* self) {
    any_mark(self->substream);
}

struct Any* stream_next(struct D_Stream* self) {
    //return METHOD_TABLE[self->streamTypeId]->m_streamNext(self);
    (void)self;
    return NULL;
}

bool stream_readChar(struct D_Stream* self, char* c) {
    struct Any* substream = self->substream;
    enum TypeId substreamId = any_typeId(substream);
    return METHOD_TABLE[substreamId]->m_streamReadChar((struct Any*)substream, c);
}

void stream_show(struct D_Stream* self, FILE* fp) {
    fprintf(fp, "Stream{");
    any_show(self->substream, fp);
    fputc('}', fp);
}

size_t stream_sizeOf(struct D_Stream* self) {
    (void)self;
    return sizeof(struct D_Stream);
}

size_t stream_structSize(void) {
    return sizeof(struct D_Stream);
}

struct Any* stream_typeOf(struct D_Stream* self) {
    (void)self;
    char* typeName = TYPE_NAMES[T_Stream];
    return (struct Any*)symbol_new(typeName);
}

bool stream_writeChar(struct D_Stream* self, char c) {
    enum TypeId substreamId = any_typeId(self->substream);
    bool res = METHOD_TABLE[substreamId]->m_streamWriteChar((struct Any*)self->substream, c);
    return res;
}

bool stream_writeString(struct D_Stream* self, struct D_String* string) {
    for (int n=0; n<string_count(string); n++) {
        char c = string_getChar_unsafe(string, n);
        if (!stream_writeChar(self, c)) {
            return false;
        }
    }
    return true;
}
