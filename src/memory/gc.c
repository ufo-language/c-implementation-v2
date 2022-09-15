#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "memory/gc.h"
#include "main/setup.h"

// The _permanentObjects pointer is an optimization. Sweeping stops
// when it reaches the beginning of the permanent objects list, so
// permanent objects never need to be connected to a root object. They
// will not be swept even if they do not get marked.

// The evaluator monitors this variable and calls gc_collect() when it
// becomes true.
bool GC_NEEDED = false;

static struct Any* _committed = NULL;
static struct Any* _permanentObjects = NULL;
static struct Any* _roots = NULL;
static struct Any* _spine = NULL;

size_t objsCreatedTrigger = 1024;
size_t bytesAllocatedTrigger = 64*1024*1024;

static size_t _totalCollections = 0;

static size_t _nObjsResident = 0;
static size_t _nBytesResident = 0;

static size_t _totalObjsCreated = 0;
static size_t _totalBytesAllocated = 0;
static size_t _totalObjsFreed = 0;
static size_t _totalBytesFreed = 0;

// these values are reset each time GC happens
static size_t _iterObjsCreated = 0;
static size_t _iterBytesAllocated = 0;
static size_t _iterObjsFreed = 0;
static size_t _iterBytesFreed = 0;

static void gc_resetLists(void);

//====================================================================
// Useful for debugging some GC problems.
//
#if 0
struct Any* _addresses[1024] = {0};

void gc_saveAddress(struct Any* addr) {
    for (int n=0; n<1024; n++) {
        if (_addresses[n] == 0) {
            _addresses[n] = addr;
            return;
        }
    }
}

void gc_removeAddress(struct Any* addr) {
    for (int n=0; n<1024; n++) {
        if (_addresses[n] == addr) {
            _addresses[n] = 0;
            return;
        }
    }
}

void gc_checkSavedAddresses(void) {
    printf("%s starting\n", __func__);
    for (int n=0; n<1024; n++) {
        if (_addresses[n] != 0) {
            printf("%s address not freed: %p, type %s ", __func__, (void*)_addresses[n], any_typeName(_addresses[n]));
            any_show(_addresses[n], stdout);
            printf("\n");
        }
    }
    printf("%s finished\n", __func__);
}
#endif
//
//====================================================================

struct Any* gc_alloc(enum TypeId typeId) {
    size_t size = any_structSize(typeId);
    struct Any* obj = (struct Any*)malloc(size);
    _nObjsResident++;
    _nBytesResident += size;  // TODO this size is wrong for size residency
                              // calculation because some classes (like Array)
                              // do an additional malloc that's not counted here
    _totalObjsCreated++;
    _iterObjsCreated++;
    _totalBytesAllocated += size;
    _iterBytesAllocated += size;
    obj->typeId = typeId;
    obj->isMarked = false;
    gc_register(obj);
    if (_iterObjsCreated >= objsCreatedTrigger) {
        // This condition can be executed many times before GC
        // actually happens because GC is started in one of the REPL
        // functions.
        GC_NEEDED = true;
    }
    if (_iterBytesAllocated >= bytesAllocatedTrigger) {
        GC_NEEDED = true;
    }
    return obj;
}

void gc_addRoot(struct Any* obj) {
    obj->next = _roots;
    _roots = obj;
}

void gc_checkBalances(void) {
    assert(_nObjsResident == _totalObjsCreated - _totalObjsFreed);
    assert(_nBytesResident == _totalBytesAllocated - _totalBytesFreed);
}

void gc_checkIncreaseTriggers(void) {
}

void gc_collect(void) {
//    _totalCollections++;
//    _iterObjsFreed = 0;
//    _iterBytesFreed = 0;
    gc_markRoots();
    gc_sweep();
    _iterObjsCreated = 0;
    _iterBytesAllocated = 0;
//    gc_checkBalances();
    GC_NEEDED = false;
//    gc_checkIncreaseTriggers();
}

void gc_commit(void) {
    _committed = _spine;
}

// Call this function only one time after instantiating all permanent
// values like 'nil' and the empty list, and interned integers.
void gc_commitPermanentObjects(void) {
    _permanentObjects = _spine;
    _committed = _spine;
}

// Move all recently added objects from the spine to the root object list.
void gc_commitRootObjects(void) {
    while (_spine != _committed) {
        struct Any* obj = _spine;
        _spine = _spine->next;
        obj->next = _roots;
        _roots = obj;
    }
}

void gc_dump(void) {
    struct Any* root = _roots;
    while (root) {
        printf("root = ");
        any_show(root, stdout);
        printf("\n");
        root = root->next;
    }
    struct Any* obj = _spine;
    int n=0;
    while (true) {
        if (obj == NULL) {
            printf("%4d) %p", n++, (void*)obj);
        }
        else {
            char* typeName = any_typeName(obj);
            printf("%4d) %p marked=%d ", n++, (void*)obj, obj->isMarked);
            any_show(obj, stdout);
            printf(" :: %s", typeName);
            if (gc_isRoot(obj)) {
                fputs(" ROOT", stdout);
            }
        }
        if (obj == _spine) fputs(" <- spine", stdout);
        if (obj == _committed) fputs(" <- committed", stdout);
        if (obj == _permanentObjects) fputs(" <- permanent", stdout);
        fputc('\n', stdout);
        if (obj == NULL) break;
        obj = obj->next;
    }
    fputc('\n', stdout);
}

// Call this function once for each object freed.
// It does not free any bytes, but it does track the number of bytes.
void gc_freeBytes(size_t bytes) {
    _nObjsResident--;
    _nBytesResident -= bytes;
    _totalObjsFreed++;
    _totalBytesFreed += bytes;
    _iterObjsFreed++;
    _iterBytesFreed += bytes;
}

// WARNING: This is O(n) on the number of committed objects.
bool gc_isCommitted(struct Any* obj) {
    struct Any* obj1 = _committed;
    while (obj1) {
        if (obj1 == obj) {
            return true;
        }
        obj1 = obj1->next;
    }
    return false;
}

// WARNING: This is O(n) on the number of root objects.
bool gc_isPermanent(struct Any* obj) {
    struct Any* obj1 = _permanentObjects;
    while (obj1) {
        if (obj == obj1) {
            return true;
        }
        obj1 = obj1->next;
    }
    return false;
}

// WARNING: This is O(n) on the number of objects in the spine.
bool gc_isSpine(struct Any* obj) {
    struct Any* obj1 = _spine;
    while (obj1) {
        if (obj == obj1) {
            return true;
        }
        obj1 = obj1->next;
    }
    return false;
}

// WARNING: This is O(n) on the number of root objects.
bool gc_isRoot(struct Any* obj) {
    struct Any* obj1 = _roots;
    while (obj1) {
        if (obj == obj1) {
            return true;
        }
        obj1 = obj1->next;
    }
    return false;
}

void gc_markRoots(void) {
    struct Any* obj = _roots;
    while (obj) {
        any_mark(obj);
        obj = obj->next;
    }
}

void gc_register(struct Any* obj) {
    obj->next = _spine;
    _spine = obj;
    obj->isMarked = false;
}

void gc_resetLists(void) {
    _committed = NULL;
    _permanentObjects = NULL;
    _roots = NULL;
    _spine = NULL;
}

void gc_start(void) {
    gc_startFunctions(setup_permanentObjects, setup_rootObjects, setup_initialize);
}

void gc_startFunctions(void (*setupPermanentObjects)(void),
                       void (*setupRootObjects)(void),
                       void (*initialize)(void)) {
    gc_resetLists();
    if (setupPermanentObjects != NULL) {
        setupPermanentObjects();
    }
    gc_commitPermanentObjects();
    if (setupRootObjects != NULL) {
        setupRootObjects();
    }
    gc_commitRootObjects();
    if (initialize != NULL) {
        initialize();
    }
}

void gc_showStats(void) {
    printf("WARNING: these numbers are not correct\n");
    printf("[Current]\n");
    printf("      Collections: %lu\n", (unsigned long)_totalCollections);
    printf(" Resident objects: %lu\n", (unsigned long)_nObjsResident);
    printf("   Resident bytes: %lu\n", (unsigned long)_nBytesResident);
    printf("  Objects trigger: %lu\n", (unsigned long)objsCreatedTrigger);
    printf("    Bytes trigger: %lu\n", (unsigned long)bytesAllocatedTrigger);
    printf("[Total]\n");
    printf("  Objects created: %lu\n", (unsigned long)_totalObjsCreated);
    printf("  Bytes allocated: %lu\n", (unsigned long)_totalBytesAllocated);
    printf("    Objects freed: %lu\n", (unsigned long)_totalObjsFreed);
    printf("      Bytes freed: %lu\n", (unsigned long)_totalBytesFreed);
    printf("[Since last GC]\n");
    printf("  Objects created: %lu\n", (unsigned long)_iterObjsCreated);
    printf("  Bytes allocated: %lu\n", (unsigned long)_iterBytesAllocated);
    printf("    Objects freed: %lu\n", (unsigned long)_iterObjsFreed);
    printf("      Bytes freed: %lu\n", (unsigned long)_iterBytesFreed);
}

void gc_stop(void) {
    struct Any* obj = _spine;
    while (obj) {
        struct Any* next = obj->next;
        any_free(obj);
        obj = next;
    }
    obj = _roots;
    while (obj) {
        struct Any* next = obj->next;
        any_free(obj);
        obj = next;
    }
    gc_resetLists();
    //gc_checkSavedAddresses();
}

void gc_sweep(void) {
    struct Any* root = _roots;
    while (root != NULL) {
        root->isMarked = false;
        root = root->next;
    }
    struct Any* prev = NULL;
    struct Any* obj = _spine;
    // these are the non-committed objects
    // just unmark them but do not sweep them
    while (obj != _committed) {
        obj->isMarked = false;
        prev = obj;
        obj = obj->next;
    }
    // these are the committed objects
    // they may be swept
    while (obj != _permanentObjects) {
        struct Any* next = obj->next;
        if (obj->isMarked) {
            obj->isMarked = false;
            prev = obj;
        }
        else {
            // disconnect and delete the object
            if (prev == NULL) {
                _spine = next;
            }
            else {
                prev->next = next;
            }
            if (obj == _committed) {
                _committed = next;
            }
            any_free(obj);
        }
        obj = next;
    }
}

// TODO I don't like this function but the REPL uses it.
// See if it can be refactored.
void gc_transferToRoots(struct Any* obj) {
    // check that the object is the last object added to the spine
    if (_spine != obj) {
        fputs("ERROR: object is not at top of spine: ", stderr);
        any_show(obj, stderr);
        fputc('\n', stderr);
        exit(1);
    }
    // remove object from the spine
    _spine = _spine->next;
    // add object to the roots
    gc_addRoot(obj);
}
