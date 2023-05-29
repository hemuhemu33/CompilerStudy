typedef struct Node Node;

typedef struct Token Token;

typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NUM,
  ND_EQ, // "=="
  ND_NE, // "!="
  ND_LT, // "<"
  ND_LE, // "<="
  ND_GT, // ">"
  ND_GE, // ">="
} NodeKind;

typedef enum{
  TK_RESERVED, // identifer
  TK_NUM,      // integer token
  TK_EOF,      // token that express end of input
} TokenKind;



struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
};


struct Token
{
  TokenKind kind; // Token
  Token *next;    // next Token
  int   val;      // integer
  char *str;      // token string
  int len;
};


extern char *user_input;
extern Token *token;
extern Token *tokenize(char *p);
extern Node *expr();
extern void gen(Node *node);
