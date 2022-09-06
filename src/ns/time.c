#include <time.h>

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

static void _cpuTime(struct Evaluator* etor, struct D_List* args);
static void _now(struct Evaluator* etor, struct D_List* args);

extern char* tzname[];
void tzset(void);

void ns_time_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "cpuTime", _cpuTime);
    primitive_define(nsHash, "now", _now);
}

static void _cpuTime(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    struct D_Real* timeReal = real_new((double)clock() / (double)CLOCKS_PER_SEC);
    evaluator_pushObj(etor, (struct Any*)timeReal);
}

static void _now(struct Evaluator* etor, struct D_List* args) {
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
