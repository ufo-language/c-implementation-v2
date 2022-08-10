#if !defined(COLONCOMMAND_H)
#define COLONCOMMAND_H

#include <stdbool.h>

#include "repl.h"

struct D_List;
struct REPL;

enum ReadAction colonCommand_run(struct D_List* tokens, struct REPL* repl);

#endif
