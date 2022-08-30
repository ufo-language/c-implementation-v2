#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/primitive.h"
#include "data/queue.h"
#include "data/string.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "os"

static void _cwd(struct Evaluator* etor, struct D_List* args);
static void _dir(struct Evaluator* etor, struct D_List* args);

void ns_os_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "cwd", _cwd);
    primitive_define(nsHash, "dir", _dir);
}

static void _cwd(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    char* buf = getcwd(NULL, 0);
    struct D_String* cwdString = string_new_move(buf, strlen(buf));
    evaluator_pushObj(etor, (struct Any*)cwdString);
}

static void _dir(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String};
    struct Any* dirNameObj = NULL;
    struct Any** paramVars[] = {&dirNameObj};
    primitive_checkArgs2(0, 1, paramTypes, args, paramVars, etor);
    char* dirName = ".";
    if (dirNameObj != NULL) {
        dirName = string_getChars((struct D_String*)dirNameObj);
    }
    struct D_Queue* dirNameQ = queue_new();
    struct dirent *dir;
    DIR* d = opendir(dirName);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (!strcmp(".", dir->d_name)) {
                continue;
            }
            if (!strcmp("..", dir->d_name)) {
                continue;
            }
            struct D_String* dirEntry = string_new(dir->d_name);
            queue_enq(dirNameQ, (struct Any*)dirEntry);
        }
        closedir(d);
    }
    evaluator_pushObj(etor, (struct Any*)queue_asList(dirNameQ));
}
