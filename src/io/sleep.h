#if !defined(SLEEP_H)
#define SLEEP_H

struct D_Real;
struct Evaluator;

void io_sleep_nonBlocking(struct Evaluator* etor, struct D_Real* delay);

#endif
