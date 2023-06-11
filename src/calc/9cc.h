typedef struct Node Node;

typedef struct Token Token;

typedef struct LVar LVar;

struct LVar {
  LVar *next;
  char *name;
  int  len;
  int  offset;
};




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
  /* ND_SC, // ";" SC:semi colon */

  ND_LVAR,    // local variable
  ND_ASSIGN,  // =
  ND_RETURN,
  
  ND_IF,
  ND_FOR,
  ND_WHILE,
} NodeKind;

typedef enum{
  TK_RESERVED, // identifer
  TK_RETURN,   // return
  TK_IF,       // if
  TK_ELSE,
  TK_FOR,      // for
  TK_WHILE,    // while
  TK_IDENT,    // identifier
  TK_NUM,      // integer token
  TK_EOF,      // token that express end of input
} TokenKind;



struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
  int offset;
  Node *cond;
  Node *initial;
  Node *exitcond;
  Node *loop;
  Node *then;
  Node *els;
};


struct Token
{
  TokenKind kind; // Token
  Token *next;    // next Token
  int   val;      // integer
  char *str;      // token string
  int len;        // token length
};
extern unsigned int Lend_number;
#define CODE_COLUMN 100

extern char *user_input;
extern Token *token;
extern Token *tokenize(char *p);
extern Node *expr();
extern void gen(Node *node);
extern void error(char *fmt, ...);
extern Node *code[CODE_COLUMN];
extern void program();
extern LVar *locals;
