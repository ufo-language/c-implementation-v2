#if !defined(QUEUE_H)
#define QUEUE_H

#include <stdbool.h>
#include <stdio.h>

struct Any;
struct D_Iterator;
struct D_Queue;
struct D_Set;
struct Evaluator;

struct D_Queue* queue_new(void);
void queue_free(struct D_Queue* self);

struct D_List* queue_asList(struct D_Queue* self);
bool queue_boolValue(struct D_Queue* self);
int queue_compare(struct D_Queue* self, struct D_Queue* other, struct Evaluator* etor);
int queue_count(struct D_Queue* self);
struct D_Queue* queue_deepCopy(struct D_Queue* self);
struct Any* queue_deq(struct D_Queue* self, struct Evaluator* etor);
struct Any* queue_deq_unsafe(struct D_Queue* self);
void queue_eval(struct D_Queue* self, struct Evaluator* etor);
void queue_enq(struct D_Queue* self, struct Any* elem);
void queue_freeVars(struct D_Queue* self, struct D_Set* freeVars, struct Evaluator* etor);
struct D_List* queue_getTail(struct D_Queue* self);
bool queue_isEmpty(struct D_Queue* self);
bool queue_isEqual(struct D_Queue* self, struct D_Queue* other);
struct D_Iterator* queue_iterator(struct D_Queue* self);
void queue_markChildren(struct D_Queue* self);
struct D_Triple* queue_match(struct D_Queue* self, struct Any* other, struct D_Triple* bindings);
struct Any* queue_peek(struct D_Queue* self);
void queue_show(struct D_Queue* self, FILE* fp);
size_t queue_sizeOf(struct D_Queue* self);
size_t queue_structSize(void);

#endif
