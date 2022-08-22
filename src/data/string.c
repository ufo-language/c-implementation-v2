#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "data/queue.h"
#include "data/string.h"
#include "data/symbol.h"
#include "dispatch/methodtable.h"
#include "etor/evaluator.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "main/typedefs.h"
#include "utils/hash.h"

struct D_String {
    struct Any obj;
    int count;
    char* chars;
};

struct Methods* string_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))string_boolValue;
    methods->m_compare = (int (*)(struct Any*, struct Any*, struct Evaluator* etor))string_compare;
    methods->m_display = (void (*)(struct Any*, FILE*))string_display;
    methods->m_free = (void (*)(struct Any*))string_free;
    methods->m_hashCode = (HashCode (*)(struct Any*, struct Evaluator*))string_hashCode;
    methods->m_isEqual = (bool (*)(struct Any*, struct Any*))string_isEqual;
    methods->m_show = (void (*)(struct Any*, FILE*))string_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))string_sizeOf;
    methods->m_structSize = string_structSize;
    return methods;
}

struct D_String* string_fromChar(char c) {
    char cs[2] = {c, 0};
    return string_new(cs);
}

struct D_String* string_new(char* str) {
    struct D_String* self = (struct D_String*)gc_alloc(T_String);
    self->count = (int)strlen(str);
    self->chars = (char*)malloc(self->count + 1);
    strcpy(self->chars, str);
    return self;
}

// str:   the char* to attach to this String
// count: the number of characters in this string, not counting
//        the null terminator
struct D_String* string_new_move(char* str, int count) {
    struct D_String* self = (struct D_String*)gc_alloc(T_String);
    self->count = count;
    self->chars = str;
    return self;
}

void string_free(struct D_String* self) {
    free(self->chars);
    free(self);
}

bool string_boolValue(struct D_String* self) {
    return self->count > 0;
}

int string_compare(struct D_String* self, struct D_String* other, struct Evaluator* etor) {
    (void)etor;
    int res = strcmp(self->chars, other->chars);
    return (res == 0) ? 0 : ((res < 0) ? -1 : 1);
}

int string_count(struct D_String* self) {
    return self->count;
}

void string_display(struct D_String* self, FILE* fd) {
    printf("%s called\n", __func__);
    fputs(self->chars, fd);
}

void string_escapifyChar(char c, FILE* fd) {
    switch (c) {
    case '"' : fputs("\\\"", fd); break;
    case '\\': fputs("\\\\", fd); break;
    case '\b': fputs("\\b", fd); break;
    case '\f': fputs("\\f", fd); break;
    case '\n': fputs("\\n", fd); break;
    case '\r': fputs("\\r", fd); break;
    case '\t': fputs("\\t", fd); break;
    default:
        fputc(c, fd);
    }
}

void string_escapify(struct D_String* self, FILE* fd) {
    for (int n=0; n<self->count; n++) {
        char c = self->chars[n];
        string_escapifyChar(c, fd);
    }
}

char string_getChar(struct D_String* self, int n, struct Evaluator* etor) {
    if (n < 0 || n >= self->count) {
        evaluator_throwException(
            etor,
            symbol_new("String"),
            "string index out of bounds",
            (struct Any*)array_newN(2, (struct Any*)integer_new(n), (struct Any*)self)
        );
    }
    return self->chars[n];
}

char string_getChar_unsafe(struct D_String* self, int n) {
    return self->chars[n];
}

char* string_getChars(struct D_String* self) {
    return self->chars;
}

HashCode string_hashCode(struct D_String* self, struct Evaluator* etor) {
    (void)etor;
    HashCode hashCode = 0;
    for (int n=0; n<self->count; n++) {
        hashCode = hashRotateLeft(hashCode) ^ self->chars[n];
    }
    return hashCode ^ HASH_PRIMES[T_String];
}

struct D_String* string_join(struct D_String* self, struct D_String* other) {
    int newLen = self->count + other->count;
    char* chars = (char*)malloc(newLen + 1);
    strcpy(chars, self->chars);
    strcpy(&chars[self->count], other->chars);
    return string_new_move(chars, newLen);
}

bool string_isEqual(struct D_String* self, struct D_String* other) {
    if (self->count != other->count) {
        return false;
    }
    for (int n=0; n<self->count; n++) {
        if (self->chars[n] != other->chars[n]) {
            return false;
        }
    }
    return true;
}

void string_show(struct D_String* self, FILE* fd) {
    fputc('"', fd);
    string_escapify(self, fd);
    fputc('"', fd);
}

struct D_List* string_split(struct D_String* self, char c) {
    struct D_Queue* queue = queue_new();
    int count = self->count;
    int startIndex = 0;
    int endIndex = 0;
    char* chars = self->chars;
    while (endIndex <= count) {
        if (c == chars[endIndex] || chars[endIndex] == 0) {
            int len = endIndex - startIndex;
            struct D_String* token;
            if (len == 0) {
                token = EMPTY_STRING;
            }
            else {
                char* substring = (char*)malloc(len + 1);
                strncpy(substring, &chars[startIndex], len);
                substring[len] = 0;
                token = string_new_move(substring, len);
            }
            queue_enq(queue, (struct Any*)token);
            startIndex = ++endIndex;
        }
        else {
            endIndex++;
        }
    }
    return queue_asList(queue);
}

size_t string_sizeOf(struct D_String* self) {
    return sizeof(self) + sizeof(self->chars);
}

size_t string_structSize(void) {
    return sizeof(struct D_String);
}

void string_unescapify(struct D_String* self, FILE* fd) {
    bool escaped = false;
    for (int n=0; n<self->count; n++) {
        char c = self->chars[n];
        if (escaped) {
            switch(c) {
            case '\"': fputc('\"', fd); break;
            case '\\': fputc('\\', fd); break;
            case 'b' : fputc('\b', fd); break;
            case 'f' : fputc('\f', fd); break;
            case 'n' : fputc('\n', fd); break;
            case 'r' : fputc('\r', fd); break;
            case 't' : fputc('\t', fd); break;
            default:
                fputc(c, fd);
            }
            escaped = false;
        }
        else if (c == '\\') {
            escaped = true;
        }
        else {
            fputc(c, fd);
        }
    }
}
