#include "data/any.h"
#include "data/tcpclient.h"
#include "etor/evaluator.h"
#include "main/globals.h"
#include "memory/gc.h"
#include "methods/methods.h"

struct D_TCPClient {
    struct Any obj;
    int port;
};

struct Methods* tcpClient_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))tcpClient_boolValue;
    methods->m_free = (void (*)(struct Any*))tcpClient_free;
    methods->m_markChildren = (void (*)(struct Any* self))tcpClient_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))tcpClient_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))tcpClient_sizeOf;
    //methods->m_streamReadChar = (bool (*)(struct Any*, char*))tcpClient_readChar;
    methods->m_structSize = tcpClient_structSize;
    //methods->m_streamWriteChar = (bool (*)(struct Any* self, char c))tcpClient_writeChar;
    return methods;
}

struct D_TCPClient* tcpClient_new(struct D_String* server, int port) {
    struct D_TCPClient* self = (struct D_TCPClient*)gc_alloc(T_File);
    (void)server;
    (void)port;
    // TODO
    return self;
}

void tcpClient_free(struct D_TCPClient* self) {
    free(self);
}

bool tcpClient_boolValue(struct D_TCPClient* self) {
    return tcpClient_isOpen(self);
}

void tcpClient_close(struct D_TCPClient* self, struct Evaluator* etor) {
    /*
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
    */
}

bool tcpClient_isOpen(struct D_TCPClient* self) {
    //return self->fp == NULL ? false : true;
    return false;
}

void tcpClient_markChildren(struct D_TCPClient* self) {
    //any_mark((struct Any*)self->fileName);
}

void tcpClient_open(struct D_TCPClient* self, struct Evaluator* etor) {
    /*
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
*/
}

bool tcpClient_readChar(struct D_TCPClient* self, char* c) {
    /*
    int c1 = fgetc(self->fp);
    if (c1 == EOF) {
        return false;
    }
    *c = c1;
    */
    return false;
}

void tcpClient_show(struct D_TCPClient* self, FILE* fp) {
    fprintf(fp, "TCPClient{...}");
}

size_t tcpClient_sizeOf(struct D_TCPClient* self) {
    return sizeof(self);
}

size_t tcpClient_structSize(void) {
    return sizeof(struct D_TCPClient);
}

bool tcpClient_writeChar(struct D_TCPClient* self, char c) {
    //fputc(c, self->fp);
    return false;
}
