#include "data/hashtable.h"
#include "data/queue.h"
#include "etor/threadmanager.h"

static struct D_Queue* _running;
static struct D_HashTable* _blocked;

void threadManager_rootObjects(void) {
    _running = queue_new();
    _blocked = hashTable_new();
}
