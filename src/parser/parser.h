#if !defined(PARSER_H)
#define PARSER_H

struct D_List;
struct Evaluator;

struct Any* parser_parse(struct D_List** tokens, struct Evaluator* etor);

#endif
