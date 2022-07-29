#if !defined(LEXER2_H)
#define LEXER2_H

enum LexerTokenType {
    LT_None,
    LT_Boolean,
    LT_Identifier,
    LT_Integer,
    LT_Nothing,
    LT_Operator,
    LT_Real,
    LT_Reserved,
    LT_Special,
    LT_String,
    LT_Symbol,
    LT_EOI,
    LT_FINAL
};

struct LexerState2 {
    char* inputString;
    int pos;
    int col;
    int line;
    char prevChar;
};

struct ResultToken {
    enum LexerTokenType tokenType;
    char* error;
    int pos;
    int col;
    int line;
    char* strValue;  // TODO rename this "lexeme"
    int strLen;
    // lexeme values: only one will be needed
    // TODO make these three fields a union?
    int intValue;
    int charValue;
    double doubleValue;
};

char* NOTHING_WORD;
char* TRUE_WORD;
char* FALSE_WORD;

void lex_nextToken(struct LexerState2* lexerState, struct ResultToken* result);

#endif
