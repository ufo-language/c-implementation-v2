#if !defined(RECORD_H)
#define RECORD_H

#include <stdbool.h>
#include <stdio.h>

struct D_Array;
struct D_Queue;
struct D_Set;
struct D_Symbol;
struct D_Record;
struct D_Triple;
struct E_Identifier;
struct E_RecordDefinition;
struct Evaluator;

struct D_Record* record_new(struct E_RecordDefinition* recordDefinition, struct D_Array* values);
void record_free(struct D_Record* self);

void record_eval(struct D_Record* self, struct Evaluator* etor);
void record_freeVars(struct D_Record* self, struct D_Set* freeVars, struct Evaluator* etor);
struct D_Record* record_deepCopy(struct D_Record* self);
struct E_RecordDefinition* record_getDefinition(struct D_Record* self);
struct Any* record_getFieldValue(struct D_Record* self, struct Any* fieldName, struct Evaluator* etor);
bool record_isEqual(struct D_Record* self, struct D_Record* other);
void record_markChildren(struct D_Record* self);
struct D_Triple* record_match(struct D_Record* self, struct Any* other, struct D_Triple* bindings);
void record_setFieldValue(struct D_Record* self, struct Any* fieldName, struct Any* value, struct Evaluator* etor);
void record_show(struct D_Record* self, FILE* fp);
size_t record_sizeOf(struct D_Record* self);
size_t record_structSize(void);
struct Any* record_typeOf(struct D_Record* self);

#endif
