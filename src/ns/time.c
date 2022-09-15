#include <sys/time.h>  // for struct timeval
#include <time.h>  // for CLOCKS_PER_SEC

#include "data/any.h"
#include "data/boolean.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/primitive.h"
#include "data/real.h"
#include "data/string.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "time"

static void _cpu(struct Evaluator* etor, struct D_List* args);
static void _current(struct Evaluator* etor, struct D_List* args);
static void _elapsed(struct Evaluator* etor, struct D_List* args);
static void _now(struct Evaluator* etor, struct D_List* args);
static void _string(struct Evaluator* etor, struct D_List* args);

void ns_time_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "cpu", _cpu);
    primitive_define(nsHash, "current", _current);
    primitive_define(nsHash, "elapsed", _elapsed);
    primitive_define(nsHash, "now", _now);
    primitive_define(nsHash, "string", _string);
}

static void _cpu(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    struct D_Real* timeReal = real_new((double)clock() / (double)CLOCKS_PER_SEC);
    evaluator_pushObj(etor, (struct Any*)timeReal);
}

static void _current(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    time_t t = time(NULL);
    struct tm *tmStruct = localtime(&t);
    struct D_HashTable* timeHash = hashTable_new();
    hashTable_putSymInt(timeHash, "Second", tmStruct->tm_sec);
    hashTable_putSymInt(timeHash, "Minute", tmStruct->tm_min);
    hashTable_putSymInt(timeHash, "Hour", tmStruct->tm_hour);
    hashTable_putSymInt(timeHash, "DayOfMonth", tmStruct->tm_mday);
    hashTable_putSymInt(timeHash, "Month", tmStruct->tm_mon);
    hashTable_putSymInt(timeHash, "Year", tmStruct->tm_year + 1900);
    hashTable_putSymInt(timeHash, "Weekday", tmStruct->tm_wday);
    hashTable_putSymInt(timeHash, "DayOfYear", tmStruct->tm_yday);
    hashTable_put_unsafe(timeHash, (struct Any*)symbol_new("IsDST"), (struct Any*)boolean_from(tmStruct->tm_isdst));
    evaluator_pushObj(etor, (struct Any*)timeHash);
}

static void _elapsed(struct Evaluator* etor, struct D_List* args) {
    static enum TypeId paramTypes[] = {T_List};
    struct Any* listObj;
    struct Any** paramVars[] = {&listObj};
    primitive_checkArgs(1, paramTypes, args, paramVars, etor);
    struct D_List* pair = (struct D_List*)listObj;
    static enum TypeId paramTypes2[] = {T_Integer, T_Integer};
    struct Any* secObj;
    struct Any* usecObj;
    struct Any** paramVars2[] = {&secObj, &usecObj};
    primitive_checkArgs(2, paramTypes2, pair, paramVars2, etor);
    struct D_Integer* secInt = (struct D_Integer*)secObj;
    struct D_Integer* usecInt = (struct D_Integer*)usecObj;
    int sec1 = integer_getValue(secInt);
    int usec1 = integer_getValue(usecInt);
    struct timeval te;
    gettimeofday(&te, NULL);
    int secElapsed = te.tv_sec - sec1;
    int usecElapsed = te.tv_usec - usec1;
    if (usecElapsed < 0) {
        secElapsed--;
        usecElapsed += 1e6;
    }
    struct D_List* elapsedPair = list_new2((struct Any*)integer_new(secElapsed),
                                           (struct Any*)integer_new(usecElapsed));
    evaluator_pushObj(etor, (struct Any*)elapsedPair);
}

static void _now(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    struct timeval te;
    gettimeofday(&te, NULL);
    struct D_List* pair = list_new2((struct Any*)integer_new(te.tv_sec),
                                    (struct Any*)integer_new(te.tv_usec));
    evaluator_pushObj(etor, (struct Any*)pair);
}

static void _string(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    time_t t = time(NULL);
    struct D_String* timeString = string_new(asctime(localtime(&t)));
    evaluator_pushObj(etor, (struct Any*)timeString);
}
