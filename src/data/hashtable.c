#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/hashtable.h"
#include "data/integer.h"
#include "data/set.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "utils/hash.h"

#define NUM_INITIAL_BUCKETS 8

struct BucketLink {
    struct Any* key;
    struct Any* value;
    struct BucketLink* next;
};

struct D_HashTable {
    struct Any obj;
    struct BucketLink** buckets;
    int count;
    int nBuckets;
    int loadingFactor;
};

static struct D_HashTable* _fold(struct D_HashTable* self, struct D_HashTable* (*func)(struct BucketLink* link, struct D_HashTable* hash, struct Evaluator* etor), struct D_HashTable* newHash, struct Evaluator* etor);
static void _forEach(struct D_HashTable* self, void (*func)(struct BucketLink* link));
static struct BucketLink* _locateLink(struct BucketLink** buckets, int nBuckets, struct Any* key, unsigned int* bucketNum, struct Evaluator* etor);
struct D_HashTable* hashTable_newNBuckets(int nBuckets);
static void _resize(struct D_HashTable* self, struct Evaluator* etor);

// allocate and free bucket links ------------------------------------

// TODO get enable bucket recycling (I don't remember if it works or not, I think it does)
#define RECYCLE_BUCKETS 0

#if RECYCLE_BUCKETS
static struct BucketLink* _allBuckets = NULL;
#endif

static struct BucketLink* _bucketLink_new(struct Any* key, struct Any* value, struct BucketLink* next) {
    struct BucketLink* link;
#if RECYCLE_BUCKETS
    if (_allBuckets != NULL) {
        link = _allBuckets;
        _allBuckets = _allBuckets->next;
    }
    else {
        link = (struct BucketLink*)malloc(sizeof(struct BucketLink));
    }
#else
    link = (struct BucketLink*)malloc(sizeof(struct BucketLink));
#endif
    link->key = key;
    link->value = value;
    link->next = next;
    return link;
}

static void _bucketLink_free(struct BucketLink* bucketLink) {
#if RECYCLE_BUCKETS
    // add the bucket to the bucket cache
    bucketLink->next = _allBuckets;
    _allBuckets = bucketLink;
#else
    free(bucketLink);
#endif
}

// free all buckets from the bucket cache
// called from ufo_stop()
void bucketLink_freeAll(void) {
#if RECYCLE_BUCKETS
    while (_allBuckets != NULL) {
        struct BucketLink* next = _allBuckets->next;
        free(_allBuckets);
        _allBuckets = next;
    }
#endif
}

// hash table functions ----------------------------------------------

struct D_HashTable* hashTable_new(void) {
    return hashTable_newNBuckets(NUM_INITIAL_BUCKETS);
}

struct D_HashTable* hashTable_newNBuckets(int nBuckets) {
    struct D_HashTable* self = (struct D_HashTable*)gc_alloc(T_HashTable);
    self->count = 0;
    self->nBuckets = nBuckets;
    self->loadingFactor = (self->nBuckets >> 1) + (self->nBuckets >> 2);
    self->buckets = (struct BucketLink**)malloc(sizeof(struct BucketLink*) * self->nBuckets);
    for (int n=0; n<self->nBuckets; n++) {
        self->buckets[n] = NULL;
    }
    return self;
}

static void _free(struct BucketLink* link) {
    free(link);
}

void hashTable_free(struct D_HashTable* self) {
    _forEach(self, _free);
    free(self->buckets);
    free(self);
}

bool hashTable_boolValue(struct D_HashTable* self) {
    return self->count > 0;
}

int hashTable_count(struct D_HashTable* self) {
    return self->count;
}

void hashTable_contin(struct Evaluator* etor, struct Any* arg) {
    int count = integer_getValue((struct D_Integer*)arg);
    struct D_HashTable* hash = hashTable_new();
    for (int n=0; n<count; n++) {
        struct Any* value = evaluator_popObj(etor);
        struct Any* key = evaluator_popObj(etor);
        hashTable_put(hash, key, value, etor);
    }
    evaluator_pushObj(etor, (struct Any*)hash);
}

static struct D_HashTable* _add(struct BucketLink* link, struct D_HashTable* hash, struct Evaluator* etor) {
    hashTable_put(hash, link->key, link->value, etor);
    return hash;
}

struct D_HashTable* hashTable_deepCopy(struct D_HashTable* self) {
    struct D_HashTable* newHash = hashTable_new();
    _fold(self, _add, newHash, NULL);
    return newHash;
}
       
void hashTable_eval(struct D_HashTable* self, struct Evaluator* etor) {
    struct D_Integer* hashCount = integer_new(self->count);
    evaluator_pushExpr(etor, (struct Any*)continuation_new(hashTable_contin, "hashTable", (struct Any*)hashCount));
    for (int n=0; n<self->nBuckets; n++) {
        struct BucketLink* link = self->buckets[n];
        while (link) {
            evaluator_pushExpr(etor, link->value);
            evaluator_pushExpr(etor, link->key);
            link = link->next;
        }
    }
}

static struct D_HashTable* _fold(struct D_HashTable* self, struct D_HashTable* (*func)(struct BucketLink* link, struct D_HashTable* hash, struct Evaluator* etor), struct D_HashTable* newHash, struct Evaluator* etor) {
    for (int n=0; n<self->nBuckets; n++) {
        struct BucketLink* link = self->buckets[n];
        while (link) {
            struct BucketLink* next = link->next;
            newHash = func(link, newHash, etor);
            link = next;
        }
    }
    return self;
}

static void _forEach(struct D_HashTable* self, void (*func)(struct BucketLink* link)) {
    for (int n=0; n<self->nBuckets; n++) {
        struct BucketLink* link = self->buckets[n];
        while (link) {
            struct BucketLink* next = link->next;
            func(link);
            link = next;
        }
    }
}

void hashTable_freeVars(struct D_HashTable* self, struct D_Set* freeVars, struct Evaluator* etor) {
    for (int n=0; n<self->nBuckets; n++) {
        struct BucketLink* link = self->buckets[n];
        while (link) {
            any_freeVars(link->key, freeVars, etor);
            any_freeVars(link->value, freeVars, etor);
            link = link->next;
        }
    }
}

struct Any* hashTable_get(struct D_HashTable* self, struct Any* key, struct Evaluator* etor) {
    unsigned int bucketNum = 0;
    struct BucketLink* link = _locateLink(self->buckets, self->nBuckets, key, &bucketNum, etor);
    if (link == NULL) {
        evaluator_throwException(
            etor,
            any_typeSymbol((struct Any*)self),
            "key not found",
            (struct Any*)array_newN(2, (struct Any*)key, (struct Any*)self)
        );
    }
    return link->value;
}

struct Any* hashTable_get_aux(struct D_HashTable* self, struct Any* key, struct Evaluator* etor) {
    unsigned int bucketNum = 0;
    struct BucketLink* link = _locateLink(self->buckets, self->nBuckets, key, &bucketNum, etor);
    return link ? link->value : NULL;
}

struct Any* hashTable_get_unsafe(struct D_HashTable* self, struct Any* key) {
    unsigned int bucketNum = 0;
    struct BucketLink* link = _locateLink(self->buckets, self->nBuckets, key, &bucketNum, NULL);
    return link ? link->value : NULL;
}

static struct BucketLink* _locateLink(struct BucketLink** buckets, int nBuckets, struct Any* key, unsigned int* bucketNum, struct Evaluator* etor) {
    HashCode hashCode = any_hashCode(key, etor);
    *bucketNum = hashCode % nBuckets;
    struct BucketLink* link = buckets[*bucketNum];
    while (link != NULL) {
        if (any_isEqual(key, link->key)) {
            return link;
        }
        link = link->next;
    }
    return NULL;
}

bool hashTable_hasKey(struct D_HashTable* self, struct Any* key, struct Evaluator* etor) {
    unsigned int bucketNum = 0;
    struct BucketLink* link = _locateLink(self->buckets, self->nBuckets, key, &bucketNum, etor);
    return link != NULL;
}

bool hashTable_isEqual(struct D_HashTable* self, struct D_HashTable* other) {
    (void)self;
    (void)other;
    printf("%s is not implenented\n", __func__);
    return false;
}

struct D_Array* hashTable_keyArray(struct D_HashTable* self) {
    struct D_Array* keyAry = array_new(self->count);
    int aryIndex = 0;
    for (int n=0; n<self->nBuckets; n++) {
        struct BucketLink* link = self->buckets[n];
        while (link) {
            array_set_unsafe(keyAry, aryIndex++, link->key);
            link = link->next;
        }
    }
    return keyAry;
}

static void _markLink(struct BucketLink* link) {
    any_mark(link->key);
    any_mark(link->value);
}

void hashTable_markChildren(struct D_HashTable* self) {
    _forEach(self, _markLink);
}

int hashTable_nBuckets(struct D_HashTable* self) {
    return self->nBuckets;
}

void hashTable_put(struct D_HashTable* self, struct Any* key, struct Any* value, struct Evaluator* etor) {
    unsigned int bucketNum = 0;
    struct BucketLink* link = _locateLink(self->buckets, self->nBuckets, key, &bucketNum, etor);
    if (link != NULL) {
        link->value = value;
    }
    else {
        if (self->count == self->loadingFactor) {
            _resize(self, etor);
            hashTable_put(self, key, value, etor);
        }
        else {
            struct Any* keyCopy = any_deepCopy(key);
            link = _bucketLink_new(keyCopy, value, self->buckets[bucketNum]);
            self->buckets[bucketNum] = link;
            self->count++;
        }
    }
}

void hashTable_put_unsafe(struct D_HashTable* self, struct Any* key, struct Any* value) {
    hashTable_put(self, key, value, NULL);
}

static void _resize(struct D_HashTable* self, struct Evaluator* etor) {
    int newNBuckets = self->nBuckets + (self->nBuckets >> 1);
    // create a new hash table with an expanded number of buckets
    struct D_HashTable* newHash = hashTable_newNBuckets(newNBuckets);
    // add each binding from the original hash table to the new hash table
    _fold(self, _add, newHash, etor);
    // swap all properties of the two hash tables
    struct BucketLink** newBuckets = newHash->buckets;
    int newCount = newHash->count;
    int newLoadingFactor = newHash->loadingFactor;
    newHash->buckets = self->buckets;
    newHash->count = self->count;
    newHash->nBuckets = self->nBuckets;
    newHash->loadingFactor = self->loadingFactor;
    self->buckets = newBuckets;
    self->count = newCount;
    self->nBuckets = newNBuckets;
    self->loadingFactor = newLoadingFactor;
}

bool hashTable_remove(struct D_HashTable* self, struct Any* key, struct Evaluator* etor) {
    HashCode hashCode = any_hashCode(key, etor);
    int bucketNum = hashCode % self->nBuckets;
    struct BucketLink* prev = NULL;
    struct BucketLink* link = self->buckets[bucketNum];
    while (link != NULL) {
        if (any_isEqual(key, link->key)) {
            struct BucketLink* savedLink = link;
            link = link->next;
            if (prev == NULL) {
                self->buckets[bucketNum] = link;
            }
            else {
                prev->next = link;
            }
            self->count--;
            _bucketLink_free(savedLink);
            return true;
        }
        link = link->next;
    }
    return false;
}

void hashTable_show(struct D_HashTable* self, FILE* fp) {
    fputs("#{", fp);
    bool firstIter = true;
    for (int n=0; n<self->nBuckets; n++) {
        struct BucketLink* link = self->buckets[n];
        while (link) {
            if (firstIter) {
                firstIter = false;
            }
            else {
                fputs(", ", fp);
            }
            struct BucketLink* next = link->next;
            any_show(link->key, fp);
            fputc('=', fp);
            any_show(link->value, fp);
            link = next;
        }
    }
    fputc('}', fp);
}

size_t hashTable_sizeOf(struct D_HashTable* self) {
    return sizeof(self) + sizeof(self->buckets);
}

size_t hashTable_structSize(void) {
    return sizeof(struct D_HashTable);
}
