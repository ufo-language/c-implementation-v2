#if !defined(EVALUATOR_H)
#define EVALUATOR_H

#include <stdbool.h>
#include <setjmp.h>
#include <stdio.h>

struct Any;
struct D_HashTable;
struct D_Symbol;
struct Evaluator;
struct E_Identifier;

struct Evaluator* evaluator_new(void);
void evaluator_initialize(struct Evaluator* self);
void evaluator_free(struct Evaluator* self);

struct D_Triple* evaluator_bind(struct Evaluator* self, struct E_Identifier* key, struct Any* value);
void evaluator_clearException(struct Evaluator* self);
void evaluator_exit(struct Evaluator* self, int exitCode);
struct D_Triple* evaluator_getEnv(struct Evaluator* self);
struct Any* evaluator_getException(struct Evaluator* self);
struct D_Triple* evaluator_getEStack(struct Evaluator* self);
struct D_List* evaluator_getOStack(struct Evaluator* self);
struct D_HashTable* evaluator_getGlobalEnv(struct Evaluator* self);
void evaluator_getJumpBuf(struct Evaluator* self, jmp_buf* jumpBuf);
struct D_HashTable* evaluator_getRecordNamespace(struct Evaluator* self);
struct D_HashTable* evaluator_getSubscriberTable(struct Evaluator* self);
struct Any* evaluator_lookup(struct Evaluator* self, struct E_Identifier* key);
void evaluator_markChildren(struct Evaluator* self);
struct Any* evaluator_popExpr(struct Evaluator* self);
void evaluator_pushExpr(struct Evaluator* self, struct Any* obj);
void evaluator_pushExprEnv(struct Evaluator* self, struct Any* expr, struct Any* env);
struct Any* evaluator_popObj(struct Evaluator* self);
void evaluator_pushObj(struct Evaluator* self, struct Any* obj);
void evaluator_reassignBinding(struct Evaluator* self, struct E_Identifier* ident, struct Any* value);
void evaluator_run(struct Evaluator* self);
void evaluator_saveEnv(struct Evaluator* self);
void evaluator_setEnv(struct Evaluator* self, struct D_Triple* env);
void evaluator_setJumpBuf(struct Evaluator* self, jmp_buf* jumpBuf);
void evaluator_setShowSteps(struct Evaluator* self, bool showSteps);
void evaluator_setSubscriberTable(struct Evaluator* self, struct D_HashTable* subscriberTable);
void evaluator_show(struct Evaluator* self, FILE* fp);
size_t evaluator_sizeOf(struct Evaluator* self);
size_t evaluator_structSize(void);
void evaluator_throwException(struct Evaluator* self, struct D_Symbol* symbol, char* message, struct Any* obj);
void evaluator_throwExceptionObj(struct Evaluator* self, struct Any* exceptionObj);
struct Any* evaluator_topExpr(struct Evaluator* self);

#endif
