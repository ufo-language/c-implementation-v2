#include <float.h>
#include <limits.h>

#include "data/any.h"
#include "data/hashtable.h"
#include "data/primitive.h"
#include "data/real.h"
#include "etor/evaluator.h"
#include "expr/identifier.h"

#define NS_NAME "real"

void ns_real_defineAll(struct D_HashTable* env) {
    struct E_Identifier* nsName = identifier_new(NS_NAME);
    struct D_HashTable* nsHash = hashTable_new();
    hashTable_put_unsafe(env, (struct Any*)nsName, (struct Any*)nsHash);
#if defined(__GNUC__) || defined(__clang__)
    hashTable_put_unsafe(nsHash, (struct Any*)identifier_new("epsilon"), (struct Any*)real_new(__DBL_EPSILON__));
    hashTable_put_unsafe(nsHash, (struct Any*)identifier_new("max"), (struct Any*)real_new(__DBL_MAX__));
    hashTable_put_unsafe(nsHash, (struct Any*)identifier_new("min"), (struct Any*)real_new(__DBL_MIN__));
#endif
#if defined(__TINYC__)
#endif
}
