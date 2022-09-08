#if !defined(HASHTABLE_H)
#define HASHTABLE_H

#include <stdbool.h>
#include <stdio.h>

struct Any;
struct D_Array;
struct D_Iterator;
struct D_Set;
struct D_HashTable;
struct Evaluator;

struct D_HashTable* hashTable_new(void);
void hashTable_free(struct D_HashTable* self);

struct D_List* hashTable_asList(struct D_HashTable* self);
bool hashTable_boolValue(struct D_HashTable* self);
int hashTable_count(struct D_HashTable* self);
struct D_HashTable* hashTable_deepCopy(struct D_HashTable* self);
void hashTable_eval(struct D_HashTable* self, struct Evaluator* etor);
void hashTable_freeVars(struct D_HashTable* self, struct D_Set* freeVars, struct Evaluator* etor);
struct Any* hashTable_get(struct D_HashTable* self, struct Any* key);
struct Any* hashTable_get_throw(struct D_HashTable* self, struct Any* key, struct Evaluator* etor);
bool hashTable_hasKey(struct D_HashTable* self, struct Any* key);
bool hashTable_isEqual(struct D_HashTable* self, struct D_HashTable* other);
struct D_Iterator* hashTable_iterator(struct D_HashTable* self);
struct D_Array* hashTable_keyArray(struct D_HashTable* self);
void hashTable_markChildren(struct D_HashTable* self);
int hashTable_nBuckets(struct D_HashTable* self);
void hashTable_put(struct D_HashTable* self, struct Any* key, struct Any* value, struct Evaluator* etor);
void hashTable_putSymInt(struct D_HashTable* hash, char* symName, int n);
void hashTable_put_unsafe(struct D_HashTable* self, struct Any* key, struct Any* value);
bool hashTable_remove(struct D_HashTable* self, struct Any* key);
void hashTable_show(struct D_HashTable* self, FILE* fp);
size_t hashTable_sizeOf(struct D_HashTable* self);
size_t hashTable_structSize(void);

#endif
