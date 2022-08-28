#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/stream.h"
#include "data/symbol.h"
#include "gc/gc.h"
#include "methods/methods.h"

struct D_Stream {
    struct Any obj;
    struct Any* substream;
    struct Methods* substreamMethods;
};

struct Methods* stream_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))stream_boolValue;
    methods->m_free = (void (*)(struct Any*))stream_free;
    methods->m_markChildren = (void (*)(struct Any* self))stream_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))stream_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))stream_sizeOf;
    methods->m_structSize = stream_structSize;
    return methods;
}

struct D_Stream* stream_new(struct Any* substream) {
    struct D_Stream* self = (struct D_Stream*)gc_alloc(T_Stream);
    self->substream = substream;
    self->substreamMethods = METHOD_TABLE[substream->typeId];
    return self;
}

void stream_free(struct D_Stream* self) {
    free(self);
}

bool stream_boolValue(struct D_Stream* self) {
    return self->substreamMethods->m_boolValue(self->substream);
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

struct Any* stream_typeOf(struct Any* self) {
    (void)self;
    char* typeName = TYPE_NAMES[T_Stream];
    return (struct Any*)symbol_new(typeName);
}
