#include <time.h>

#include "data/any.h"
#include "data/boolean.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/list.h"
#include "data/primitive.h"
#include "data/string.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"
#include "main/globals.h"

#define NS_NAME "time"

static void _now(struct Evaluator* etor, struct D_List* args);

void ns_time_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
    primitive_define(nsHash, "now", _now);
}

extern char** tzname;
void tzset(void);

static void _now(struct Evaluator* etor, struct D_List* args) {
    primitive_checkArgs(0, NULL, args, NULL, etor);
    time_t t = time(NULL);
    struct tm *tmStruct = localtime(&t);
    int sec = tmStruct->tm_sec;
    int min = tmStruct->tm_min;
    int hour = tmStruct->tm_hour;
    int mday = tmStruct->tm_mday;
    int mon = tmStruct->tm_mon;
    int year = tmStruct->tm_year;
    int wday = tmStruct->tm_wday;
    int yday = tmStruct->tm_yday;
    int isdst = tmStruct->tm_isdst;
    struct D_HashTable* timeHash = hashTable_new();
    hashTable_put_unsafe(timeHash, (struct Any*)symbol_new("Second"), (struct Any*)integer_new(sec));
    hashTable_put_unsafe(timeHash, (struct Any*)symbol_new("Minute"), (struct Any*)integer_new(min));
    hashTable_put_unsafe(timeHash, (struct Any*)symbol_new("Hour"), (struct Any*)integer_new(hour));
    hashTable_put_unsafe(timeHash, (struct Any*)symbol_new("DayOfMonth"), (struct Any*)integer_new(mday));
    hashTable_put_unsafe(timeHash, (struct Any*)symbol_new("Month"), (struct Any*)integer_new(mon));
    hashTable_put_unsafe(timeHash, (struct Any*)symbol_new("Year"), (struct Any*)integer_new(year+1900));
    hashTable_put_unsafe(timeHash, (struct Any*)symbol_new("Weekday"), (struct Any*)integer_new(wday));
    hashTable_put_unsafe(timeHash, (struct Any*)symbol_new("DayOfYear"), (struct Any*)integer_new(yday));
    hashTable_put_unsafe(timeHash, (struct Any*)symbol_new("IsDST"), (struct Any*)boolean_from(isdst));
    evaluator_pushObj(etor, (struct Any*)timeHash);
}
