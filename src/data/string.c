#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "data/iterator.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/string.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "main/typedefs.h"  // for HashCode
#include "methods/methods.h"
#include "utils/hash.h"
#include "utils/strtok.h"

struct D_String {
    struct Any obj;
    int count;
    char* chars;
};

struct Any* string_getPairValue(struct D_String* self, struct Any* prefix);
bool string_iteratorHasNext(struct D_Iterator* iterator);
struct Any* string_iteratorNext(struct D_Iterator* iterator);

struct Methods* string_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))string_boolValue;
    methods->m_compare = (int (*)(struct Any*, struct Any*, struct Evaluator* etor))string_compare;
    methods->m_display = (void (*)(struct Any*, FILE*))string_display;
    methods->m_free = (void (*)(struct Any*))string_free;
    methods->m_getPairValue = (struct Any* (*)(struct Any*, struct Any*))string_getPairValue;
    methods->m_hashCode = (bool (*)(struct Any*, HashCode*))string_hashCode;
    methods->m_isEqual = (bool (*)(struct Any*, struct Any*))string_isEqual;
    methods->m_iterator = (struct D_Iterator* (*)(struct Any*))string_iterator;
    methods->m_iteratorHasNext = string_iteratorHasNext;
    methods->m_iteratorNext = string_iteratorNext;
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

struct Any* string_getPairValue(struct D_String* self, struct Any* prefix) {
    if (T_String != any_typeId(prefix)) {
        return NULL;
    }
    struct D_String* prefixString = (struct D_String*)prefix;
    return string_startsWith(self, prefixString) ? (struct Any*)self : NULL;
}

bool string_hashCode(struct D_String* self, HashCode* hashCode) {
    HashCode charHashCode = 0;
    for (int n=0; n<self->count; n++) {
        charHashCode = hashRotateLeft(charHashCode) ^ self->chars[n];
    }
    *hashCode = charHashCode ^ HASH_PRIMES[T_String];
    return true;
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

struct D_Iterator* string_iterator(struct D_String* self) {
    struct D_List* pair = list_new((struct Any*)integer_new(0), (struct Any*)self);
    return iterator_new((struct Any*)pair, T_String);
}

bool string_iteratorHasNext(struct D_Iterator* iterator) {
    struct D_List* iterObj = (struct D_List*)iterator_getStateObject(iterator);
    struct D_Integer* indexObj = (struct D_Integer*)list_getFirst(iterObj);
    struct D_String* string = (struct D_String*)list_getRest(iterObj);
    return integer_getValue(indexObj) < string->count;
}

struct Any* string_iteratorNext(struct D_Iterator* iterator) {
    struct D_List* iterObj = (struct D_List*)iterator_getStateObject(iterator);
    struct D_Integer* indexObj = (struct D_Integer*)list_getFirst(iterObj);
    struct D_String* string = (struct D_String*)list_getRest(iterObj);
    int index = integer_getValue(indexObj);
    if (index < string->count) {
        struct D_String* chr = string_fromChar(string->chars[index]);
        list_setFirst(iterObj, (struct Any*)integer_new(index + 1));
        return (struct Any*)chr;
    }
    return NULL;
}

void string_show(struct D_String* self, FILE* fd) {
    fputc('"', fd);
    string_escapify(self, fd);
    fputc('"', fd);
}

size_t string_sizeOf(struct D_String* self) {
    return sizeof(self) + sizeof(self->chars);
}

struct D_List* string_split(struct D_String* self, struct D_String* delim) {
    char* str = string_getChars(self);
    char* del = string_getChars(delim);
    struct D_Queue* stringQ = queue_new();
    char* rest = NULL;
    char* token;
    for (token = strtok_r(str, del, &rest); token != NULL; token = strtok_r(NULL, del, &rest)) {   
        queue_enq(stringQ, (struct Any*)string_new(token));
    }
    return queue_asList(stringQ);
}

bool string_startsWith(struct D_String* self, struct D_String* prefix) {
    if (self->count < prefix->count) {
        return false;
    }
    char* selfChars = self->chars;
    char* prefixChars = prefix->chars;
    for (int n=0; n<prefix->count; n++) {
        if (selfChars[n] != prefixChars[n]) {
            return false;
        }
    }
    return true;
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
