#if !defined(READLINE_H)
#define READLINE_H

struct D_StringBuffer;
struct Evaluator;

void io_readLine_nonBlocking(struct Evaluator* etor, struct D_StringBuffer* sb);

#endif
