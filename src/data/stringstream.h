#if !defined(STRINGSTREAM_H)
#define STRINGSTREAM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

struct D_StringStream* stringStream_new(char* str);
void stringStream_free(struct D_StringStream* self);

bool stringStream_boolValue(struct D_StringStream* self);
int stringStream_count(struct D_StringStream* self);
int stringStream_get(struct D_StringStream* self);
void stringStream_getPos(struct D_StringStream* self, int* pos, int* col, int* line);
void stringStream_show(struct D_StringStream* self, FILE* fp);
void stringStream_unget(struct D_StringStream* self);
size_t stringStream_sizeOf(struct D_StringStream* self);
size_t stringStream_structSize();

#endif
