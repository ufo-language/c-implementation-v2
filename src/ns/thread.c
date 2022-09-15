#include "data/any.h"
#include "data/array.h"
#include "data/boolean.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/primitive.h"
#include "data/real.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "etor/threadmanager.h"
#include "expr/apply.h"
#include "expr/identifier.h"
#include "io/sleep.h"
#include "main/globals.h"

#define NS_NAME "thread"

static void _count(struct Evaluator* etor, struct D_List* args);
static void _isTerminated(struct Evaluator* etor, struct D_List* args);
static void _join(struct Evaluator* etor, struct D_List* args);
static void _self(struct Evaluator* etor, struct D_List* args);
static void _sleep(struct Evaluator* etor, struct D_List* args);
static void _spawn(struct Evaluator* etor, struct D_List* args);
static void _status(struct Evaluator* etor, struct D_List* args);
static void _value(struct Evaluator* etor, struct D_List* args);

void ns_thread_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "count", _count);
    primitive_define(nsHash, "isTerminated", _isTerminated);
    primitive_define(nsHash, "join", _join);
    primitive_define(nsHash, "self", _self);
    primitive_define(nsHash, "sleep", _sleep);
    primitive_define(nsHash, "spawn", _spawn);
    primitive_define(nsHash, "status", _status);
    primitive_define(nsHash, "value", _value);
}

static void _count(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    struct D_Integer* count = integer_new(threadManager_count());
    evaluator_pushObj(etor, (struct Any*)count);
}

static void _isTerminated(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Evaluator};
    struct Any* etorObj;
    struct Any** paramVars[] = {&etorObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct Evaluator* thread = (struct Evaluator*)etorObj;
    enum ThreadStatus status = evaluator_getThreadStatus(thread);
    evaluator_pushObj(etor, (struct Any*)boolean_from(TS_Terminated == status));
}

static void _join(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Evaluator};
    struct Any* etorObj;
    struct Any** paramVars[] = {&etorObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct Evaluator* thread = (struct Evaluator*)etorObj;
    if (evaluator_getThreadStatus(thread) != TS_Terminated) {
        threadManager_blockThread(etor, (struct Any*)thread);
    }
    evaluator_pushObj(etor, (struct Any*)NIL);
}

static void _self(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    evaluator_pushObj(etor, (struct Any*)etor);
}

static void _sleep(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Integer, T_Real};
    struct Any* delayObj = NULL;
    enum TypeId typeId = primitive_checkArgsOneOf(2, paramTypes, args, &delayObj, etor);
    if (typeId == T_Integer) {
        delayObj = (struct Any*)real_new(integer_getValue((struct D_Integer*)delayObj));
    }
    io_sleep_nonBlocking(etor, (struct D_Real*)delayObj);
}

static void _spawn(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_NULL, T_List};
    struct Any* expr;
    struct Any* argObj = (struct Any*)EMPTY_LIST;
    struct Any** paramVars[] = {&expr, &argObj};
    primitive_checkArgs2(1, 2, paramTypes, args, paramVars, etor);
    struct E_Apply* app = apply_new(expr, (struct D_List*)argObj);
    struct Evaluator* etorNew = evaluator_new();
    evaluator_pushExpr(etorNew, (struct Any*)app);
    threadManager_addThread(etorNew);
    evaluator_pushObj(etor, (struct Any*)etorNew);
}

static void _status(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Evaluator};
    struct Any* etorObj;
    struct Any** paramVars[] = {&etorObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct Evaluator* thread = (struct Evaluator*)etorObj;
    enum ThreadStatus status = evaluator_getThreadStatus(thread);
    struct D_Symbol* statusSymbol = threadManager_statusSymbol(status);
    evaluator_pushObj(etor, (struct Any*)statusSymbol);
}

static void _value(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_Evaluator};
    struct Any* etorObj;
    struct Any** paramVars[] = {&etorObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct Evaluator* thread = (struct Evaluator*)etorObj;
    enum ThreadStatus status = evaluator_getThreadStatus(thread);
    if (TS_Terminated == status) {
        struct D_List* ostack = evaluator_getOStack(thread);
        struct Any* value = list_getFirst(ostack);
        evaluator_pushObj(etor, (struct Any*)value);
        return;
    }
    evaluator_throwException(etor, symbol_new("Thread"), "thread is not terminated", (struct Any*)thread);
}
