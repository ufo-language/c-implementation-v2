#if !defined(REPL_H)
#define REPL_H

#include <stdbool.h>
#include <stdio.h>

#include "data/any.h"

struct D_List;
struct D_String;
struct D_StringBuffer;
struct Evaluator;

// TODO should this be in repl.c instead?
struct REPL {
    struct Any obj;
    struct D_StringBuffer* inputStringBuffer;
    struct D_String* inputString;
    struct D_String* string;
    struct D_List* tokens;
    struct D_List* colonTokens;
    struct Any* expr;
    struct Any* value;
    struct Evaluator* etor;
    struct Any* error;
    bool fileWasLoaded;
    struct D_String* lastFileLoaded;
};

struct REPL* repl_new(bool makeRoot);
void repl_free(struct REPL* self);

struct D_String* repl_getInputString(struct REPL* self);
void repl_markChildren(struct REPL* self);
void repl_run(struct REPL* self);
void repl_show(struct REPL* self, FILE* fp);
size_t repl_sizeOf(struct REPL* self);
size_t repl_structSize();

#endif
