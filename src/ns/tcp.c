#include "data/any.h"
#include "data/hashtable.h"
#include "data/primitive.h"
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

void ns_tcp_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    //primitive_define(nsHash, "systemInfo", _sysinfo);
}

#if 0
static void _dir(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String};
    struct Any* dirNameObj = NULL;
    struct Any** paramVars[] = {&dirNameObj};
    primitive_checkArgs2(0, 1, paramTypes, args, paramVars, etor);
}
#endif
