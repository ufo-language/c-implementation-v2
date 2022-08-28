#if !defined(STREAM_H)
#define STREAM_H

#include <stdio.h>

struct D_Stream;
struct Evaluator;

struct D_Stream* stream_new(void);
void stream_free(struct D_Stream* self);

bool stream_boolValue(struct D_Stream* self);
void stream_markChildren(struct D_Stream* self);
void stream_show(struct D_Stream* self, FILE* fp);
size_t stream_sizeOf(struct D_Stream* self);
size_t stream_structSize(void);
struct Any* stream_typeOf(struct Any* self);

#endif
