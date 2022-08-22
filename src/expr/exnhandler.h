#if !defined(EXCEPTIONHANDLER_H)
#define EXCEPTIONHANDLER_H

#include <stdbool.h>
#include <stdio.h>

struct Any;
struct D_List;
struct D_Triple;
struct Evaluator;
struct E_Abstraction;
struct E_ExceptionHandler;

struct E_ExceptionHandler* exceptionHandler_new(struct E_Abstraction* catchExpr, struct D_List* ostack, struct D_Triple* env);
void exceptionHandler_free(struct E_ExceptionHandler* self);

bool exceptionHandler_apply(struct E_ExceptionHandler* self, struct Any* exception, struct Evaluator* etor);
void exceptionHandler_eval(struct E_ExceptionHandler* self, struct Evaluator* etor);
void exceptionHandler_markChildren(struct E_ExceptionHandler* self);
void exceptionHandler_show(struct E_ExceptionHandler* self, FILE* fd);
size_t exceptionHandler_sizeOf(struct E_ExceptionHandler* self);
size_t exceptionHandler_structSize(void);

#endif
