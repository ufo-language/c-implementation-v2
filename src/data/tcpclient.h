#if !defined(TCPCLIENT_H)
#define TCPCLIENT_H

#include <stdbool.h>
#include <stdio.h>

#include "any.h"

struct D_String;
struct D_TCPClient;
struct Evaluator;

struct D_TCPClient* tcpClient_new(struct D_String* server, int port);
void tcpClient_free(struct D_TCPClient* self);

bool tcpClient_boolValue(struct D_TCPClient* self);
void tcpClient_close(struct D_TCPClient* self, struct Evaluator* etor);
bool tcpClient_isOpen(struct D_TCPClient* self);
void tcpClient_markChildren(struct D_TCPClient* self);
void tcpClient_show(struct D_TCPClient* self, FILE* fp);
size_t tcpClient_sizeOf(struct D_TCPClient* self);
size_t tcpClient_structSize(void);

#endif
