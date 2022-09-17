#include <string.h>

#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/primitive.h"
#include "data/string.h"
#include "data/tcpclient.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "tcp"

/*
   tcp:connect(address :: String, port :: Integer) :: TCPClient
   tcp:serve(port :: Integer) :: TCPServer
   tcp:accept(server :: TCPServer) :: TCPClient
   tcp:readString(client :: TCPClient) :: String
   tcp:writeString(client :: TCPClient) :: Nil
   tcp:close(client :: TCPClient) :: Nil
   tcp:close(server :: TCPServer) :: Nil
*/

static void _accept(struct Evaluator* etor, struct D_List* args);
static void _client(struct Evaluator* etor, struct D_List* args);
static void _close(struct Evaluator* etor, struct D_List* args);
static void _connect(struct Evaluator* etor, struct D_List* args);
static void _readString(struct Evaluator* etor, struct D_List* args);
static void _serve(struct Evaluator* etor, struct D_List* args);
static void _writeString(struct Evaluator* etor, struct D_List* args);

void ns_tcp_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "accept", _accept);
    primitive_define(nsHash, "client", _client);
    primitive_define(nsHash, "close", _close);
    primitive_define(nsHash, "connect", _connect);
    primitive_define(nsHash, "readString", _readString);
    primitive_define(nsHash, "serve", _serve);
    primitive_define(nsHash, "writeString", _writeString);
}

// working
static void _client(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String, T_Integer};
    struct Any* addressObj = NULL;
    struct Any* portObj = NULL;
    struct Any** paramVars[] = {&addressObj, &portObj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_String* addressString = (struct D_String*)addressObj;
    struct D_Integer* portInt = (struct D_Integer*)portObj;
    struct D_TCPClient* tcpClient = tcpClient_new(addressString, integer_getValue(portInt));
    evaluator_pushObj(etor, (struct Any*)tcpClient);
}

// working
static void _connect(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_TCPClient};
    struct Any* tcpClientObj = NULL;
    struct Any** paramVars[] = {&tcpClientObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_TCPClient* tcpClient = (struct D_TCPClient*)tcpClientObj;
    int errno = tcpClient_connect(tcpClient);
    if (errno != 0) {
        struct D_Symbol* sym = symbol_new("TCPClient");
        evaluator_throwException(etor, sym, strerror(errno), (struct Any*)tcpClientObj);
    }
    evaluator_pushObj(etor, (struct Any*)tcpClientObj);
}

static void _writeString(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_TCPClient, T_String};
    struct Any* tcpClientObj = NULL;
    struct Any* stringObj = NULL;
    struct Any** paramVars[] = {&tcpClientObj, &stringObj};
    primitive_checkArgs(2, paramTypes, args, paramVars, etor);
    struct D_TCPClient* tcpClient = (struct D_TCPClient*)tcpClientObj;
    struct D_String* string = (struct D_String*)stringObj;
    int res = tcpClient_writeString(tcpClient, string);
    struct D_Integer* intRes = integer_new(res);
    evaluator_pushObj(etor, (struct Any*)intRes);
}

// incomplete functions follow

static void _accept(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String};
    struct Any* dirNameObj = NULL;
    struct Any** paramVars[] = {&dirNameObj};
    primitive_checkArgs2(0, 1, paramTypes, args, paramVars, etor);
    evaluator_pushObj(etor, (struct Any*)NIL);
}

static void _close(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String};
    struct Any* dirNameObj = NULL;
    struct Any** paramVars[] = {&dirNameObj};
    primitive_checkArgs2(0, 1, paramTypes, args, paramVars, etor);
    evaluator_pushObj(etor, (struct Any*)NIL);
}

static void _readString(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String};
    struct Any* dirNameObj = NULL;
    struct Any** paramVars[] = {&dirNameObj};
    primitive_checkArgs2(0, 1, paramTypes, args, paramVars, etor);
    evaluator_pushObj(etor, (struct Any*)NIL);
}

static void _serve(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String};
    struct Any* dirNameObj = NULL;
    struct Any** paramVars[] = {&dirNameObj};
    primitive_checkArgs2(0, 1, paramTypes, args, paramVars, etor);
    evaluator_pushObj(etor, (struct Any*)NIL);
}
