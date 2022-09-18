#if !defined(TCPCLIENTOPEN_H)
#define TCPCLIENTOPEN_H

struct D_String;
struct D_TCPClient;
struct Evaluator;

void io_tcpClient_open(struct Evaluator* etor, struct D_TCPClient* client);
void io_tcpClient_readString(struct Evaluator* etor, struct D_TCPClient* tcpClient);
void io_tcpClient_writeString(struct Evaluator* etor, struct D_TCPClient* self, struct D_String* string);

#endif
