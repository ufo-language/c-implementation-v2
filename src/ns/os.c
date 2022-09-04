#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>

#include "defines.h"
#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/primitive.h"
#include "data/queue.h"
#include "data/string.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "os"

static void _cwd(struct Evaluator* etor, struct D_List* args);
static void _dir(struct Evaluator* etor, struct D_List* args);
static void _env(struct Evaluator* etor, struct D_List* args);
static void _platform(struct Evaluator* etor, struct D_List* args);
static void _sysinfo(struct Evaluator* etor, struct D_List* args);

void ns_os_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "cwd", _cwd);
    primitive_define(nsHash, "dir", _dir);
    primitive_define(nsHash, "env", _env);
    primitive_define(nsHash, "platform", _platform);
    primitive_define(nsHash, "systemInfo", _sysinfo);
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

extern char** environ;

static void _env(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_String};
    struct Any* keyNameObj = NULL;
    struct Any** paramVars[] = {&keyNameObj};
    primitive_checkArgs2(0, 1, paramTypes, args, paramVars, etor);
    if (keyNameObj == NULL) {
        struct D_HashTable* envHash = hashTable_new();
        char** env = environ;
        struct D_String* delim = string_new("=");
        while (*env) {
            struct D_String* str = string_new(*env);
            struct D_List* parts = string_split(str, delim);
            struct D_String* key = (struct D_String*)list_getFirst(parts);
            struct D_String* value = (struct D_String*)list_getSecond(parts);
            hashTable_put_unsafe(envHash, (struct Any*)key, (struct Any*)value);
            env++;
        }
        evaluator_pushObj(etor, (struct Any*)envHash);
    }
    else {
        char* key = string_getChars((struct D_String*)keyNameObj);
        char* envValue = getenv(key);
        struct Any* value = (struct Any*)NIL;
        if (envValue) {
            value = (struct Any*)string_new(envValue);
        }
        evaluator_pushObj(etor, value);
    }
}

static void _platform(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    struct D_String* platform = string_new(OS_PLATFORM);
    evaluator_pushObj(etor, (struct Any*)platform);
}

static void _sysinfo(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    struct sysinfo si;
    int res = sysinfo(&si);
    if (res < 0) {
        evaluator_throwException(
            etor,
            symbol_new("Sysinfo"),
            "error retrieving system information",
            (struct Any*)array_newN(2, (struct Any*)symbol_new("ERRNO"),
                                       (struct Any*)integer_new(errno)));
    }
    int uptime = (int)si.uptime;       /* Seconds since boot */
    int load1 = (int)si.loads[0];      /* 1, 5, and 15 minute load averages */
    int load5 = (int)si.loads[1];      /* 1, 5, and 15 minute load averages */
    int load15 = (int)si.loads[2];     /* 1, 5, and 15 minute load averages */
    int totalRam = (int)si.totalram;   /* Total usable main memory size */
    int freeRam = (int)si.freeram;     /* Available memory size */
    int sharedRam = (int)si.sharedram; /* Amount of shared memory */
    int bufferRam = (int)si.bufferram; /* Memory used by buffers */
    int totalSwap = (int)si.totalswap; /* Total swap space size */
    int freeSwap = (int)si.freeswap;   /* Swap space still available */
    int procs = (int)si.procs;         /* Number of current processes */
    struct D_HashTable* siHash = hashTable_new();
    hashTable_putSymInt(siHash, "Uptime", uptime);
    hashTable_putSymInt(siHash, "Load1", load1);
    hashTable_putSymInt(siHash, "Load5", load5);
    hashTable_putSymInt(siHash, "Load15", load15);
    hashTable_putSymInt(siHash, "TotalRam", totalRam);
    hashTable_putSymInt(siHash, "FreeRam", freeRam);
    hashTable_putSymInt(siHash, "SharedRam", sharedRam);
    hashTable_putSymInt(siHash, "BufferRam", bufferRam);
    hashTable_putSymInt(siHash, "TotalSwap", totalSwap);
    hashTable_putSymInt(siHash, "FreeSwap", freeSwap);
    hashTable_putSymInt(siHash, "Procs", procs);
    evaluator_pushObj(etor, (struct Any*)siHash);
}
