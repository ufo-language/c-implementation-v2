#if !defined(D_STRINGBUFFER_H)
#define D_STRINGBUFFER_H

#include <stdbool.h>
#include <stdio.h>

struct Array;
struct D_StringBuffer;

struct D_StringBuffer* stringBuffer_new(void);
struct D_StringBuffer* stringBuffer_initialize(struct D_StringBuffer* self);
void stringBuffer_free(struct D_StringBuffer* self);

struct D_String* stringBuffer_asString(struct D_StringBuffer* self);
bool stringBuffer_boolValue(struct D_StringBuffer* self);
void stringBuffer_clear(struct D_StringBuffer* self);
int stringBuffer_count(struct D_StringBuffer* self);
char* stringBuffer_getChars(struct D_StringBuffer* self, int* count);
bool stringBuffer_readChar(struct D_StringBuffer* self, char* c);
void stringBuffer_show(struct D_StringBuffer* self, FILE* fd);
void stringBuffer_write(struct D_StringBuffer* self, struct D_String* string);
bool stringBuffer_writeChar(struct D_StringBuffer* self, char c);
bool stringBuffer_writeChars(struct D_StringBuffer* self, char* chars);
size_t stringBuffer_sizeOf(struct D_StringBuffer* self);
size_t stringBuffer_structSize(void);

#endif
