#if !defined(COLONCOMMAND_H)
#define COLONCOMMAND_H

#include <stdbool.h>

struct D_List;
struct REPL;

bool colonCommand_run(struct D_List* tokens, struct REPL* repl);

#endif
