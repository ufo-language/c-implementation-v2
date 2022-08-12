#if !defined(RECORDSPEC_H)
#define RECORDSPEC_H

#include <stdio.h>

struct Any;
struct D_Queue;
struct D_Set;
struct D_Symbol;
struct D_RecordSpec;
struct D_Triple;
struct Evaluator;

struct E_RecordSpec* recordSpec_new(struct D_Symbol* recordSpecTypeName, struct D_Queue* fieldBindings);
void recordSpec_free(struct E_RecordSpec* self);

void recordSpec_eval(struct E_RecordSpec* self, struct Evaluator* etor);
void recordSpec_freeVars(struct E_RecordSpec* self, struct D_Set* freeVars, struct Evaluator* etor);
void recordSpec_markChildren(struct E_RecordSpec* self);
struct D_Triple* recordSpec_match(struct E_RecordSpec* self, struct Any* other, struct D_Triple* bindings);
void recordSpec_show(struct E_RecordSpec* self, FILE* fp);
size_t recordSpec_sizeOf(struct E_RecordSpec* self);
size_t recordSpec_structSize(void);

#endif
