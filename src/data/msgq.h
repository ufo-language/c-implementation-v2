#if !defined(MSGQ_H)
#define MSGQ_H

#include <stdio.h>

struct Any;
struct D_MsgQ;
struct Evaluator;

struct D_MsgQ* msgq_new(void);
void msgq_initialize(struct D_MsgQ* self);
void msgq_free(struct D_MsgQ* self);

int msgq_count(struct D_MsgQ* self);
void msgq_enq(struct D_MsgQ* self, struct Any* message);
void msgq_markChildren(struct D_MsgQ* self);
void msgq_show(struct D_MsgQ* self, FILE* fp);
size_t msgq_sizeOf(struct D_MsgQ* self);
size_t msgq_structSize(void);
void msgq_wait(struct D_MsgQ* self, struct Evaluator* etor);

#endif
