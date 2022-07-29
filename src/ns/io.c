#include "data/any.h"
#include "data/hashtable.h"
#include "data/list.h"
#include "data/primitive.h"
#include "data/string.h"
#include "data/stringbuffer.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "io/readline.h"
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
    evaluator_pushObj(etor, (struct Any*)NOTHING);
}

static void _dispLn(struct Evaluator* etor, struct D_List* args) {
    _disp(etor, args);
    putchar('\n');
    evaluator_pushObj(etor, (struct Any*)NOTHING);
}

static void _nl(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    putchar('\n');
    evaluator_pushObj(etor, (struct Any*)NOTHING);
}

static void _readLine(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    struct D_StringBuffer* sb = stringBuffer_new();
    io_readLine(sb);
    evaluator_pushObj(etor, (struct Any*)stringBuffer_asString(sb));
}

static void _show(struct Evaluator* etor, struct D_List* args) {
    while (!list_isEmpty(args)) {
        any_show(list_getFirst(args), stdout);
        args = (struct D_List*)list_getRest(args);
    }
    evaluator_pushObj(etor, (struct Any*)NOTHING);
}

static void _showLn(struct Evaluator* etor, struct D_List* args) {
    _show(etor, args);
    putchar('\n');
    evaluator_pushObj(etor, (struct Any*)NOTHING);
}
