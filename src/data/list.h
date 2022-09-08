#if !defined(D_LIST_H)
#define D_LIST_H

#include <stdbool.h>
#include <stdio.h>

struct Any;
struct D_Iterator;
struct D_List;
struct D_Set;
struct D_Triple;
struct Evaluator;

struct D_List* list_new(struct Any* first, struct Any* rest);
struct D_List* list_new2(struct Any* first, struct Any* second);
void list_free(struct D_List* self);

struct D_Array* list_asArray(struct D_List* self);
bool list_boolValue(struct D_List* self);
int list_compare(struct D_List* self, struct D_List* other, struct Evaluator* etor);
// returns -1 if the list is improper
int list_count(struct D_List* self);
struct D_List* list_deepCopy(struct D_List* self);
void list_eval(struct D_List* self, struct Evaluator* etor);
void list_freeVars(struct D_List* self, struct D_Set* freeVars, struct Evaluator* etor);
struct Any* list_getFirst(struct D_List* self);
struct Any* list_getRest(struct D_List* self);
struct D_List* list_getRestAsList(struct D_List* self);
struct Any* list_getSecond(struct D_List* self);
struct Any* list_getNth(struct D_List* self, int n, struct Evaluator* etor);
bool list_isEmpty(struct D_List* self);
bool list_isEqual(struct D_List* self, struct D_List* other);
struct D_Iterator* list_iterator(struct D_List* self);
void list_markChildren(struct D_List* self);
struct D_Triple* list_match(struct D_List* self, struct Any* other, struct D_Triple* bindings);
void list_setFirst(struct D_List* self, struct Any* first);
void list_setRest(struct D_List* self, struct Any* rest);
void list_show(struct D_List* self, FILE* fp);
void list_showWith(struct D_List* self, char* open, char* sep, char* close, FILE* fp);
size_t list_sizeOf(struct D_List* self);
size_t list_structSize(void);
struct D_Array* list_toArray(struct D_List* self, int nElems);

#endif
