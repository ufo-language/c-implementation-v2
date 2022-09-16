#include "data/any.h"
#include "data/tcpserver.h"
#include "etor/evaluator.h"
#include "main/globals.h"
#include "memory/gc.h"
#include "methods/methods.h"

struct D_TCPServer {
    struct Any obj;
    int port;
};

struct Methods* tcpServer_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))tcpServer_boolValue;
    methods->m_free = (void (*)(struct Any*))tcpServer_free;
    methods->m_markChildren = (void (*)(struct Any* self))tcpServer_markChildren;
    methods->m_show = (void (*)(struct Any*, FILE*))tcpServer_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))tcpServer_sizeOf;
    //methods->m_streamReadChar = (bool (*)(struct Any*, char*))tcpServer_readChar;
    methods->m_structSize = tcpServer_structSize;
    //methods->m_streamWriteChar = (bool (*)(struct Any* self, char c))tcpServer_writeChar;
    return methods;
}

struct D_TCPServer* tcpServer_new(int port) {
    struct D_TCPServer* self = (struct D_TCPServer*)gc_alloc(T_File);
    self->port = port;
    // TODO open the server socket
    return self;
}

void tcpServer_free(struct D_TCPServer* self) {
    free(self);
}

bool tcpServer_boolValue(struct D_TCPServer* self) {
    return tcpServer_isOpen(self);
}

void tcpServer_close(struct D_TCPServer* self, struct Evaluator* etor) {
    (void)self;
    (void)etor;
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

bool tcpServer_isOpen(struct D_TCPServer* self) {
    (void)self;
    //return self->fp == NULL ? false : true;
    return false;
}

void tcpServer_markChildren(struct D_TCPServer* self) {
    (void)self;
    //any_mark((struct Any*)self->fileName);
}

void tcpServer_open(struct D_TCPServer* self, struct Evaluator* etor) {
    (void)self;
    (void)etor;
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

void tcpServer_show(struct D_TCPServer* self, FILE* fp) {
    (void)self;
    fprintf(fp, "TCPServer{...}");
}

size_t tcpServer_sizeOf(struct D_TCPServer* self) {
    return sizeof(self);
}

size_t tcpServer_structSize(void) {
    return sizeof(struct D_TCPServer);
}
