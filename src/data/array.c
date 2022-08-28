#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/boolean.h"
#include "data/integer.h"
#include "data/iterator.h"
#include "data/list.h"
#include "data/queue.h"
#include "data/sequence.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/apply.h"
#include "expr/continuation.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "main/typedefs.h"  // for HashCode
#include "methods/methods.h"
#include "utils/hash.h"

struct D_Array {
    struct Any obj;
    int count;
    struct Any** elems;
};

bool array_boolValue(struct D_Array* self);
struct Any* array_getPairValue(struct D_Array* self, struct Any* key);
bool array_iteratorHasNext(struct D_Iterator* iterator);
struct Any* array_iteratorNext(struct D_Iterator* iterator);

struct Methods* array_methodSetup(void) {
    struct Methods* methods = (struct Methods*)malloc(sizeof(struct Methods));
    methodTable_setupDefaults(methods);
    methods->m_boolValue = (bool (*)(struct Any*))array_boolValue;
    methods->m_compare = (int (*)(struct Any*, struct Any*, struct Evaluator* etor))array_compare;
    methods->m_deepCopy = (struct Any* (*)(struct Any*))array_deepCopy;
    methods->m_eval = (void (*)(struct Any*, struct Evaluator*))array_eval;
    methods->m_free = (void (*)(struct Any*))array_free;
    methods->m_freeVars = (void (*)(struct Any*, struct D_Set*, struct Evaluator*))array_freeVars;
    methods->m_getPairValue = (struct Any* (*)(struct Any*, struct Any*))array_getPairValue;
    methods->m_hashCode = (bool (*)(struct Any*, HashCode*))array_hashCode;
    methods->m_isEqual = (bool (*)(struct Any*, struct Any*))array_isEqual;
    methods->m_iterator = (struct D_Iterator* (*)(struct Any*))array_iterator;
    methods->m_iteratorHasNext = array_iteratorHasNext;
    methods->m_iteratorNext = array_iteratorNext;
    methods->m_markChildren = (void (*)(struct Any* self))array_markChildren;
    methods->m_match = (struct D_Triple* (*)(struct Any*, struct Any*, struct D_Triple*))array_match;
    methods->m_show = (void (*)(struct Any*, FILE*))array_show;
    methods->m_sizeOf = (size_t (*)(struct Any*))array_sizeOf;
    methods->m_structSize = array_structSize;
    return methods;
}

struct D_Array* array_new(int count) {
    return array_newWith(count, (struct Any*)NIL);
}

struct D_Array* array_newN(int count, ...) {
    struct D_Array* self = array_new(count);
    va_list argList;
    va_start(argList, count);
    for (int n=0; n<count; n++) {
        struct Any* elem = va_arg(argList, struct Any*);
        self->elems[n] = elem;
    }
    va_end(argList);
    return self;
}

struct D_Array* array_newWith(int count, struct Any* elem) {
    struct D_Array* self = (struct D_Array*)gc_alloc(T_Array);
    self->count = count;
    self->elems = (struct Any**)malloc(count * sizeof(struct Any*));
    for (int n=0; n<count; n++) {
        self->elems[n] = elem;
    }
    return self;
}

void array_free(struct D_Array* self) {
    free(self->elems);
    free(self);
}

struct D_Queue* array_asQueue(struct D_Array* self) {
    struct D_Queue* q = queue_new();
    struct Any** elems = self->elems;
    for (int n=0; n<self->count; n++) {
        queue_enq(q, elems[n]);
    }
    return q;
}

bool array_boolValue(struct D_Array* self) {
    return self->count != 0;
}

int array_compare(struct D_Array* self, struct D_Array* other, struct Evaluator* etor) {
    if (self->count < other->count) {
        return -1;
    }
    if (self->count > other->count) {
        return 1;
    }
    for (int n=0; n<self->count; n++) {
        int res = any_compare(self->elems[n], other->elems[n], etor);
        if (res != 0) {
            return res;
        }
    }
    return 0;
}

bool array_contains(struct D_Array* self, struct Any* elem) {
    struct Any** elems = self->elems;
    for (int n=0; n<self->count; n++) {
        if (any_isEqual(elem, elems[n])) {
            return true;
        }
    }
    return false;
}

struct D_Array* array_copy(struct D_Array* self) {
    struct D_Array* newAry = array_new(self->count);
    for (int n=0; n<self->count; n++) {
        newAry->elems[n] = self->elems[n];
    }
    return newAry;
}

int array_count(struct D_Array* self) {
    return self->count;
}

void array_contin(struct Evaluator* etor, struct Any* arg) {
    int count = integer_getValue((struct D_Integer*)arg);
    struct D_Array* ary = array_new(count);
    for (int n=count; n>0; n--) {
        ary->elems[n-1] = evaluator_popObj(etor);
    }
    evaluator_pushObj(etor, (struct Any*)ary);
}

struct D_Array* array_deepCopy(struct D_Array* self) {
    struct D_Array* ary = array_new(self->count);
    for (int n=0; n<self->count; n++) {
        ary->elems[n] = any_deepCopy(self->elems[n]);
    }
    return ary;
}

void array_delete(struct D_Array* self, int index) {
    int n = index;
    for (; n<self->count-1; n++) {
        self->elems[n] = self->elems[n+1];
    }
    self->elems[n] = (struct Any*)NIL;
}

void array_eval(struct D_Array* self, struct Evaluator* etor) {
    struct D_Integer* arrayCount = integer_new(self->count);
    struct E_Continuation* contin = continuation_new(array_contin, "array", (struct Any*)arrayCount);
    evaluator_pushExpr(etor, (struct Any*)contin);
    for (int n=self->count; n>0; n--) {
        evaluator_pushExpr(etor, self->elems[n-1]);
    }
}

void array_freeVars(struct D_Array* self, struct D_Set* freeVars, struct Evaluator* etor) {
    for (int n=0; n<self->count; n++) {
        any_freeVars(self->elems[n], freeVars, etor);
    }
}

struct Any* array_get(struct D_Array* self, int n, struct Evaluator* etor) {
    if (n >= self->count) {
        evaluator_throwException(
            etor,
            symbol_new("Array"),
            "index out of bounds",
            (struct Any*)array_newN(2, (struct Any*)integer_new(n), (struct Any*)self)
        );
    }
    return self->elems[n];
}

struct Any* array_get_unsafe(struct D_Array* self, int n) {
    return self->elems[n];
}

struct Any* array_getAssoc(struct D_Array* self, struct Any* key) {
    struct Any** elems = self->elems;
    for (int n=0; n<self->count; n++) {
        struct Any* val = any_getPairValue(elems[n], key);
        if (val) {
            return val;
        }
    }
    return NULL;
}

struct Any* array_getPairValue(struct D_Array* self, struct Any* key) {
    if (self->count < 2) {
        return NULL;
    }
    struct Any** elems = self->elems;
    return any_isEqual(key, elems[0]) ? elems[1] : NULL;
}

struct D_Array* array_getRange(struct D_Array* self, struct D_Sequence* range) {
    int start = sequence_getFrom(range);
    if (start < 0) start = 0;
    int stop = sequence_getTo(range);
    if (stop > self->count) stop = self->count;
    int by = sequence_getBy(range);
    struct Any** elems = self->elems;
    struct D_Queue* elemQ = queue_new();
    for (int n=start; n<=stop; n+=by) {
        queue_enq(elemQ, elems[n]);
    }
    return list_asArray(queue_asList(elemQ));
}

bool array_hashCode(struct D_Array* self, HashCode* hashCode) {
    HashCode elemHashCode = 0;
    HashCode tempHashCode;
    struct Any** elems = self->elems;
    for (int n=0; n<self->count; n++) {
        if (!any_hashCode(elems[n], &tempHashCode)) {
            return false;
        }
        elemHashCode = hashRotateLeft(elemHashCode) ^ tempHashCode;
    }
    *hashCode = elemHashCode ^ HASH_PRIMES[T_Array];
    return true;
}

void array_insert(struct D_Array* self, int index, struct Any* elem, struct Any* deadZone) {
    struct Any** elems = self->elems;
    struct Any* temp = elems[index];
    for (int n=index+1; n<self->count; n++) {
        struct Any* temp2 = elems[n];
        elems[n] = temp;
        if (deadZone && any_isEqual(elems[n], deadZone)) {
            break;
        }
        temp = temp2;
    }
    elems[index] = elem;
}

struct D_Array* array_insertionSort(struct D_Array* self, struct Evaluator* etor) {
    int count = self->count;
    struct Any** elems = self->elems;
    struct D_Array* newArray = array_newWith(count, (struct Any*)NIL);
    for (int n=0; n<count; n++) {
        struct Any* elem = elems[n];
        if (n == 0) {
            newArray->elems[0] = elem;
        }
        else {
            for (int m=0; m<=n; m++) {
                struct Any* newElem = newArray->elems[m];
                if (newElem == (struct Any*)NIL) {
                    newArray->elems[m] = elem;
                    break;
                }
                else if (any_compare(elem, newElem, etor) < 0) {
                    array_insert(newArray, m, elem, (struct Any*)NIL);
                    break;
                }
            }
        }
    }
    return newArray;
}

bool array_isEqual(struct D_Array* self, struct D_Array* other) {
    if (self->count != other->count) {
        return false;
    }
    for (int n=0; n<self->count; n++) {
        if (!any_isEqual(array_get_unsafe(self, n), array_get_unsafe(other, n))) {
            return false;
        }
    }
    return true;
}

struct D_Iterator* array_iterator(struct D_Array* self) {
    struct D_Array* iterObj = array_newN(2, integer_new(0), self);
    return iterator_new((struct Any*)iterObj, T_Array);
}

bool array_iteratorHasNext(struct D_Iterator* iterator) {
    struct D_Array* iterObj = (struct D_Array*)iterator_getStateObject(iterator);
    struct D_Integer* indexObj = (struct D_Integer*)iterObj->elems[0];
    struct D_Array* elemAry = (struct D_Array*)iterObj->elems[1];
    return integer_getValue(indexObj) < elemAry->count;
}

struct Any* array_iteratorNext(struct D_Iterator* iterator) {
    struct D_Array* iterObj = (struct D_Array*)iterator_getStateObject(iterator);
    struct D_Integer* indexObj = (struct D_Integer*)iterObj->elems[0];
    struct D_Array* elemAry = (struct D_Array*)iterObj->elems[1];
    int index = integer_getValue(indexObj);
    if (index < elemAry->count) {
        struct Any* elem = elemAry->elems[index];
        iterObj->elems[0] = (struct Any*)integer_new(index + 1);
        return elem;
    }
    return NULL;
}

// builds the apply structure for the map function
struct E_Apply* array_map_apply(struct D_Array* self, int n, struct Any* abstr) {
    struct D_List* args = list_new(self->elems[n], (struct Any*)EMPTY_LIST);
    return apply_new(abstr, args);
}

// handles the continuation for the map function
void array_map_contin(struct Evaluator* etor, struct Any* arg) {
    struct D_Array* args = (struct D_Array*)arg;
    struct D_Array* self = (struct D_Array*)args->elems[0];
    struct D_Integer* indexInt = (struct D_Integer*)args->elems[1];
    struct D_Array* newArray = (struct D_Array*)args->elems[2];
    struct Any* abstr = args->elems[3];
    int index = integer_getValue(indexInt);
    struct Any* newElem = evaluator_popObj(etor);
    newArray->elems[index] = newElem;
    if (++index == newArray->count) {
        evaluator_pushObj(etor, (struct Any*)newArray);
    }
    else {
        args->elems[1] = (struct Any*)integer_new(index);
        struct E_Continuation* contin = continuation_new(array_map_contin, "array_map", (struct Any*)args);
        evaluator_pushExpr(etor, (struct Any*)contin);
        struct E_Apply* app = array_map_apply(self, index, abstr);
        evaluator_pushExpr(etor, (struct Any*)app);
    }
}

void array_map(struct D_Array* self, struct Any* abstr, struct Evaluator* etor) {
    struct D_Integer* nInt = integer_new(0);
    struct D_Array* newArray = array_new(self->count);
    struct D_Array* args = array_newN(4, (struct Any*)self, (struct Any*)nInt, (struct Any*)newArray, abstr);
    struct E_Continuation* contin = continuation_new(array_map_contin, "array_map", (struct Any*)args);
    evaluator_pushExpr(etor, (struct Any*)contin);
    struct E_Apply* app = array_map_apply(self, 0, abstr);
    evaluator_pushExpr(etor, (struct Any*)app);
}

void array_markChildren(struct D_Array* self) {
    for (int n=0; n<self->count; n++) {
        any_mark(array_get_unsafe(self, n));
    }
}

struct D_Triple* array_match(struct D_Array* self, struct Any* other, struct D_Triple* bindings) {
    if (!any_isA(other, T_Array)) {
        return NULL;
    }
    struct D_Array* otherArray = (struct D_Array*)other;
    if (self->count != otherArray->count) {
        return NULL;
    }
    for (int n=0; n<self->count; n++) {
        bindings = any_match(array_get_unsafe(self, n), array_get_unsafe(otherArray, n), bindings);
        if (bindings == NULL) {
            return NULL;
        }
    }
    return bindings;
}

void array_reverse(struct D_Array* self) {
    int count = self->count;
    struct Any** elems = self->elems;
    for (int n=0; n<(count/2); n++) {
        struct Any* temp = elems[n];
        elems[n] = elems[count - n - 1];
        elems[count - n - 1] = temp;
    }
}

struct D_Array* array_selectionSort(struct D_Array* self, struct Evaluator* etor) {
    int count = self->count;
    for (int n=0; n<count; n++) {
        struct Any* smallest = self->elems[n];
        int smallestIndex = n;
        for (int m=n+1; m<count; m++) {
            struct Any* elem = self->elems[m];
            int res = any_compare(elem, smallest, etor);
            if (res == -1) {
                smallest = elem;
                smallestIndex = m;
            }
        }
        // swap the elements found
        if (n != smallestIndex) {
            struct Any* temp = self->elems[n];
            self->elems[n] = smallest;
            self->elems[smallestIndex] = temp;
        }
    }
    return self;
}

void array_set(struct D_Array* self, int index, struct Any* elem, struct Evaluator* etor) {
    if (index >= self->count) {
        evaluator_throwException(
            etor,
            symbol_new("Array"),
            "index out of bounds",
            (struct Any*)array_newN(2, (struct Any*)integer_new(index), (struct Any*)self)
        );
    }
    self->elems[index] = elem;
}

void array_set_unsafe(struct D_Array* self, int index, struct Any* elem) {
    self->elems[index] = elem;
}

void array_show(struct D_Array* self, FILE* fp) {
    array_showWith(self, "{", ", ", "}", fp);
}

void array_showWith(struct D_Array* self, char* open, char* sep, char* close, FILE* fp) {
    fputs(open, fp);
    for (int n=0; n<self->count; n++) {
        if (n != 0) {
            fputs(sep, fp);
        }
        any_show(self->elems[n], fp);
    }
    fputs(close, fp);
}

void array_shuffle(struct D_Array* self) {
    int count = self->count;
    struct Any** elems = self->elems;
    for (int n=0; n<count; n++) {
        int nextIndex = rand() % count;
        struct Any* temp = elems[n];
        elems[n] = elems[nextIndex];
        elems[nextIndex] = temp;
    }
}

size_t array_sizeOf(struct D_Array* self) {
    return sizeof(self) + sizeof(self->elems);
}

size_t array_structSize(void) {
    return sizeof(struct D_Array);
}
