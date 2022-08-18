#if !defined(LEXER_H)
#define LEXER_H

struct D_List;
struct D_String;
struct D_Symbol;

enum LexerTokenType {
    LTT_None,
    LTT_Boolean,
    LTT_Nil,
    LTT_Identifier,
    LTT_Integer,
    LTT_Operator,
    LTT_Real,
    LTT_Reserved,
    LTT_Special,
    LTT_String,
    LTT_Symbol,
    LTT_Word,
    LTT_EOI,
    LTT_FINAL
};

extern struct D_Symbol* LEXER_SYMBOLS[];

struct D_List* lexer_tokenize(struct D_String* inputString);

#endif
