#if !defined(ARRAY_H)
#define ARRAY_H

#include <stdbool.h>
#include <stdio.h>

#include "any.h"
#include "main/typedefs.h"  // for HashCode

struct D_Array;
struct D_Queue;
struct D_Sequence;
struct D_Set;
struct Evaluator;

struct D_Array* array_new(int n);
struct D_Array* array_newN(int n, ...);
struct D_Array* array_newWith(int count, struct Any* elem);
void array_free(struct D_Array* self);

struct D_Queue* array_asQueue(struct D_Array* self);
int array_compare(struct D_Array* self, struct D_Array* other, struct Evaluator* etor);
bool array_contains(struct D_Array* self, struct Any* elem);
struct D_Array* array_copy(struct D_Array* self);
int array_count(struct D_Array* self);
struct D_Array* array_deepCopy(struct D_Array* self);
void array_delete(struct D_Array* self, int index);
void array_eval(struct D_Array* self, struct Evaluator* etor);
void array_freeVars(struct D_Array* self, struct D_Set* freeVars, struct Evaluator* etor);
struct Any* array_get(struct D_Array* self, int n, struct Evaluator* etor);
struct Any* array_get_unsafe(struct D_Array* self, int n);
struct Any* array_getAssoc(struct D_Array* self, struct Any* key);
struct D_Array* array_getRange(struct D_Array* self, struct D_Sequence* range);
bool array_hashCode(struct D_Array* self, HashCode* hashCode);
void array_insert(struct D_Array* self, int index, struct Any* elem, struct Any* deadZone);
struct D_Array* array_insertionSort(struct D_Array* self, struct Evaluator* etor);
bool array_isEqual(struct D_Array* self, struct D_Array* other);
struct D_Iterator* array_iterator(struct D_Array* self);
void array_map(struct D_Array* self, struct Any* abstr, struct Evaluator* etor);
void array_markChildren(struct D_Array* self);
struct D_Triple* array_match(struct D_Array* self, struct Any* other, struct D_Triple* bindings);
void array_reverse(struct D_Array* self);
void array_set(struct D_Array* self, int n, struct Any* elem, struct Evaluator* etor);
void array_set_unsafe(struct D_Array* self, int n, struct Any* elem);
void array_show(struct D_Array* self, FILE* fp);
void array_showWith(struct D_Array* self, char* open, char* sep, char* close, FILE* fp);
void array_shuffle(struct D_Array* self);
size_t array_sizeOf(struct D_Array* self);
struct D_Array* array_selectionSort(struct D_Array* self, struct Evaluator* etor);
size_t array_structSize(void);

#endif
