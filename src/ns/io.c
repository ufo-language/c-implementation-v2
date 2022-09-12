#include <stdlib.h>

#include "data/any.h"
#include "data/hashtable.h"
#include "data/list.h"
#include "data/primitive.h"
#include "data/string.h"
#include "data/stringbuffer.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "io/readline.h"
#include "ipc/ipc.h"
#include "main/globals.h"

#define NS_NAME "io"

static void _disp(struct Evaluator* etor, struct D_List* args);
static void _dispLn(struct Evaluator* etor, struct D_List* args);
static void _nl(struct Evaluator* etor, struct D_List* args);
static void _readLine(struct Evaluator* etor, struct D_List* args);
static void _show(struct Evaluator* etor, struct D_List* args);
static void _showLn(struct Evaluator* etor, struct D_List* args);

void ns_io_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "disp", _disp);
    primitive_define(nsHash, "displn", _dispLn);
    primitive_define(nsHash, "nl", _nl);
    primitive_define(nsHash, "readLine", _readLine);
    primitive_define(nsHash, "show", _show);
    primitive_define(nsHash, "showln", _showLn);
}

static void _disp(struct Evaluator* etor, struct D_List* args) {
    while (!list_isEmpty(args)) {
        any_display(list_getFirst(args), stdout);
        args = (struct D_List*)list_getRest(args);
    }
    evaluator_pushObj(etor, (struct Any*)NIL);
}

static void _dispLn(struct Evaluator* etor, struct D_List* args) {
    _disp(etor, args);
    putchar('\n');
}

static void _nl(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    putchar('\n');
    evaluator_pushObj(etor, (struct Any*)NIL);
}

static void _readLine(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
#if 0  // original
    struct D_StringBuffer* sb = stringBuffer_new();
    io_readLine(sb);
    evaluator_pushObj(etor, (struct Any*)stringBuffer_asString(sb));
#else
    printf("%s writing 'R' to parent\n", __func__);
    ipc_writeChar(CHILD_TO_PARENT[0], 'R');
    printf("%s waiting for number of chars read by parent\n", __func__);
    int nChars = ipc_readInt(CHILD_TO_PARENT[0]);
    printf("%s got %d\n", __func__, nChars);
    char* chars = (char*)malloc(nChars + 1);
    ipc_readString(CHILD_TO_PARENT[0], nChars, chars);
    chars[nChars] = '\0';
    struct D_String* string = string_new_move(chars, nChars);
    evaluator_pushObj(etor, (struct Any*)string);
#endif
}

static void _show(struct Evaluator* etor, struct D_List* args) {
    while (!list_isEmpty(args)) {
        any_show(list_getFirst(args), stdout);
        args = (struct D_List*)list_getRest(args);
    }
    evaluator_pushObj(etor, (struct Any*)NIL);
}

static void _showLn(struct Evaluator* etor, struct D_List* args) {
    _show(etor, args);
    putchar('\n');
}
