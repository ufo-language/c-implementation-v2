#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/any.h"
#include "data/array.h"
#include "data/integer.h"
#include "data/symbol.h"
#include "etor/evaluator.h"
#include "expr/continuation.h"
#include "gc/gc.h"
#include "main/globals.h"
#include "utils/hash.h"

struct D_Array {
    struct Any obj;
    int count;
    struct Any** elems;
};

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
    evaluator_pushExpr(etor, (struct Any*)continuation_new(array_contin, "array", (struct Any*)arrayCount));
    for (int n=self->count; n>0; n--) {
        evaluator_pushExpr(etor, self->elems[n-1]);
    }
}

void array_freeVars(struct D_Array* self, struct D_Set* freeVars, struct Evaluator* etor) {
    for (int n=0; n<self->count; n++) {
        any_freeVars(self->elems[n], freeVars, etor);
    }
}

struct Any* array_get_unsafe(struct D_Array* self, int index) {
    return self->elems[index];
}

HashCode array_hashCode(struct D_Array* self, struct Evaluator* etor) {
    HashCode hashCode = 0;
    for (int n=0; n<self->count; n++) {
        hashCode = hashRotateLeft(hashCode) ^ any_hashCode(self->elems[n], etor);
    }
    return hashCode ^ HASH_PRIMES[T_Array];
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

size_t array_sizeOf(struct D_Array* self) {
    return sizeof(self) + sizeof(self->elems);
}

// This uses a double-ended selection sort.
// TODO THIS DOES NOT WORK RIGHT
// UFO> array:sort({5,4,3,2,1})
// {2, 1, 3, 5, 4} :: Array
#if 0
struct D_Array* array_selectionSort(struct D_Array* self, struct Evaluator* etor) {
    int count = self->count;
    int nSwaps = 0;
    int nComps = 0;
    for (int n=0; n<count; n++) {
        int top = count - n - 1;
        int smallestIndex = n;
        struct Any* smallest = self->elems[smallestIndex];
        int largestIndex = top;
        struct Any* largest = self->elems[largestIndex];
        for (int m=n+1; m<top; m++) {
            if (n == top) {
                break;
            }
            struct Any* elem = self->elems[m];
            int res = any_compare(elem, smallest, etor);
            nComps++;
            if (res == -1) {
                smallest = elem;
                smallestIndex = m;
            }
            res = any_compare(elem, largest, etor);
            nComps++;
            if (res == 1) {
                largest = elem;
                largestIndex = m;
            }
        }
        // swap the elements into place
        if (n != smallestIndex) {
            struct Any* temp = self->elems[n];
            self->elems[n] = smallest;
            self->elems[smallestIndex] = temp;
            nSwaps++;
        }
        if (top != largestIndex) {
            struct Any* temp = self->elems[top];
            self->elems[top] = largest;
            self->elems[largestIndex] = temp;
            nSwaps++;
        }
    }
    printf("swaps %d, comps %d\n", nSwaps, nComps);
    return self;
}
#else
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
#endif

size_t array_structSize(void) {
    return sizeof(struct D_Array);
}
