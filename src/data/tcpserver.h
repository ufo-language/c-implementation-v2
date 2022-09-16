#if !defined(TCPSERVER_H)
#define TCPSERVER_H

#include <stdbool.h>

#include "any.h"

struct D_TCPServer;
struct Evaluator;

struct D_TCPServer* tcpServer_new(int port);
void tcpServer_free(struct D_TCPServer* self);

bool tcpServer_boolValue(struct D_TCPServer* self);
void tcpServer_close(struct D_TCPServer* self, struct Evaluator* etor);
bool tcpServer_isOpen(struct D_TCPServer* self);
void tcpServer_markChildren(struct D_TCPServer* self);
void tcpServer_show(struct D_TCPServer* self, FILE* fp);
size_t tcpServer_sizeOf(struct D_TCPServer* self);
size_t tcpServer_structSize(void);

#endif
