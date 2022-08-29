#if !defined(STREAM_H)
#define STREAM_H

#include <stdio.h>

struct D_Stream;
struct D_String;
struct Evaluator;
struct StreamMethods;

struct D_Stream* stream_new(struct Any* substream);
struct D_Stream* stream_newFrom(struct Any* obj, struct Evaluator* etor);
void stream_free(struct D_Stream* self);

bool stream_boolValue(struct D_Stream* self);
void stream_markChildren(struct D_Stream* self);
void stream_show(struct D_Stream* self, FILE* fp);
size_t stream_sizeOf(struct D_Stream* self);
size_t stream_structSize(void);
struct Any* stream_typeOf(struct D_Stream* self);
bool stream_readChar(struct D_Stream* self, char* c);
bool stream_writeChar(struct D_Stream* self, char c);
bool stream_writeString(struct D_Stream* self, struct D_String* string);

#endif
