#if !defined(FILE_H)
#define FILE_H

#include <stdbool.h>
#include <stdio.h>

#include "any.h"

struct D_File;
struct D_String;
struct D_StringBuffer;

struct D_File* file_new(struct D_String* fileName);
struct D_File* file_new_charString(char* fileName);
void file_free(struct D_File* self);

bool file_boolValue(struct D_File* self);
void file_close(struct D_File* self, struct Evaluator* etor);
bool file_isOpen(struct D_File* self);
void file_markChildren(struct D_File* self);
void file_open(struct D_File* self, struct Evaluator* etor);
bool file_open_aux(struct D_File* self);
//struct D_StringBuffer* file_readAll(struct D_File* self, struct Evaluator* etor);
struct D_Array* file_readAll(struct D_File* self, struct D_StringBuffer* stringBuffer, struct Evaluator* etor);
size_t file_readAll_stringBuffer(struct D_File* self, struct D_StringBuffer* stringBuffer, struct Evaluator* etor);
char file_readChar(struct D_File* self);
void file_show(struct D_File* self, FILE* fp);
// returns the size of the file on disk
long file_size(struct D_File* self);
// returns the size of the file object
size_t file_sizeOf(struct D_File* self);
// returns the size of the file structure
size_t file_structSize();

#endif
