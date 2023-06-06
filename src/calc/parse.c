#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>

#include "9cc.h"


Node *primary();
Node *mul();
Node *expr();
Node *assign();

LVar *locals;

int is_alphabet(char c){
  if ((('a' <= c) && (c <= 'z')) ||
      (('A' <= c) && (c <= 'Z'))) {
    return true;
  }
  return false;
}

int is_number(char c) {
  if ((('0' <= c) && (c <= '9'))) {
    return true;
  }
  return false;
}

int is_alnum(char c) {
  if (is_alphabet(c) ||
      is_number(c)   ||
      c == '_') {
    return true;
  }
  return false;
}


LVar *find_lvar(Token *tok) {
  /* printf("%s\n", tok->str); */
  /* printf("%d\n", tok->len); */
  for (LVar *var = locals; var; var = var->next) {
    if ((memcmp(var->name, tok->str,tok->len) == 0) &&
	(var->len == tok->len)) {
      return var;
      
    }
  }

  return NULL;
}


Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_identify() {

  Node *node = calloc(1, sizeof(Node));

  node->kind = ND_LVAR;
  /* node->offset = (token->str[0]-'a'+1)*8; */
  /* token=token->next; */
  LVar *lvar = find_lvar(token);


  if (lvar != NULL) {  // tokenで記載されていたstrがすでに登録されていた
    node->offset = lvar->offset;
  }
  else {       // tokenが新規登録だった

    lvar = calloc(1, sizeof(LVar)); // lvar新規生成

    lvar->next = locals;            // Local Valのリストの先頭を更新

    lvar->name = token->str;        // 変数名
    lvar->len = token->len;         // 長さを設定。。。
    lvar->offset = locals->offset + 8; //      

    node->offset = lvar->offset;    // nodeのメモリの格納先を設定
    locals = lvar;                  // Local Valのリストの先頭ポインタを更新

  }
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

char *user_input;
Token *token;

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap,fmt);

  int pos = loc - user_input;

  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " "); // input blank
  /* for (int i = 0; i < pos; i++) { */
  /*   fprintf(stderr, " "); */
  /* } */

  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error(char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool consume_ident(){
  if ((token->kind == TK_IDENT)){
    return true;
  }
  return false;
}

bool consume(char *op){
  if ((token->kind != TK_RESERVED) || (strlen(op) != (unsigned int)token->len) ||
      (memcmp(token->str, op, token->len))) {
    return false;
  }
  token = token->next;
  return true;
}

void expect(char op){
  if (token->kind != TK_RESERVED || token->str[0] != op) {
    /* return false; */
    /* error("not '%c'", op); */
    error_at(token->str,"not '%c'", op);
    
  }
  token = token->next;
}

int expect_number(){
  if (token->kind != TK_NUM) {
    /* error("it is not number"); */
    error_at(token->str, "it is not number");
  }
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof(){
  return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len){
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

Node *unary(){
  if (consume("+")) {
    return primary();
  }else if (consume("-")) {
    return new_node(ND_SUB, new_node_num(0), primary());
  }else {
    return primary();
  }
  return primary();
  
}

Node *add(){
  Node *node = mul();
  for (;;){
    if (consume("+")) {
      node = new_node(ND_ADD, node, mul());
    } else if (consume("-")) {
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
  return node;
}


Node *relational(){
  Node *node = add();
  for (;;) {
    if (consume("<")) {
      node = new_node(ND_LT, node, add());
    } else if(consume("<=")) {
      node = new_node(ND_LE, node, add());      
    } else if(consume(">")) {
      node = new_node(ND_GT, node, add());      
    } else if(consume(">=")){
      node = new_node(ND_GE, node, add());      
    } else {
      return node;
    }
  }
  return node;
}

Node *equality(){
  Node *node = relational();
  for (;;) {
    if (consume("==")) {
      node = new_node(ND_EQ, node, relational());
    } else if (consume("!=")) {
      node = new_node(ND_NE, node, relational());
    } else {
      return node;
    }
  }
  return node;

}

Node *expr(){
  Node *node = assign();
  return node;
}


Node *primary(){
  if (consume("(")) {
    Node *node = expr();
    expect(')');
    return node;
  }
  else if(consume_ident()){
    Node *node = new_node_identify();
    token=token->next;
    return node;
  }

  return new_node_num(expect_number());
}


Node *mul(){
  Node *node = unary();
  for (;;){
    if (consume("*")) {
      node = new_node(ND_MUL, node, unary());
    } else if (consume("/")) {
      node = new_node(ND_DIV, node, unary());
    } else {
      return node;
    }
    
  }
}


Node *assign(){
  Node *node = equality();
  if (consume("=") == true) {
    node = new_node(ND_ASSIGN, node, assign());   }
  return node;
}


Node *stmt() {
  Node *node = NULL;
  if (TK_RETURN == token->kind) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
  }
  else {
    node = expr();
  }
  
  /* Node *node = expr(); */
  expect(';');
  return node;
}

Node *code[CODE_COLUMN];

void program(){
  int i = 0;
  while(!at_eof()) {
    code[i++] = stmt();
  }
  code[i] = NULL; // for dummy
}



Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {

    //空白文字はskip
    if (isspace(*p)) {
      p++;
      continue;
    }

    if ((strncmp(p,"==",2) == 0) ||
	(strncmp(p,"!=",2) == 0) ||
	(strncmp(p,"<=",2) == 0) ||
	(strncmp(p,">=",2) == 0)) {
      cur = new_token(TK_RESERVED, cur, p,2);
      p = p+2;
    }
    
    if (*p == '+' || *p == '-' || *p == '*'
	|| *p == '/' || *p == '(' || *p == ')'
	|| *p == '<' || *p == '>' || *p == '='
	|| *p == ';') {
      cur = new_token(TK_RESERVED, cur, p++,1);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p,-1); // 引数lenはこれは何でもいい。
      cur->val = strtol(p, &p, 10);
      continue;
    }

    /*************************************************************************/
    /*                            returnの呼び出し                          */
    /*************************************************************************/
    // returnという文字列が有効であることを確認
    if ((strncmp(p, "return",6) == 0) && is_alnum(p[6])) {
      /* tokens[i]. */
      cur = new_token(TK_RETURN, cur, p, strlen("return"));
      p = p + 6;
      continue;
    }
    
    
    
    /*************************************************************************/
    /*                            識別子の読み取り                          */
    /*************************************************************************/
    int chLength = 0;

    for (char *start = p; start != NULL; start++) {
      if ('a' <= *start && *start <= 'z') {
	chLength++;
      }
      else {
	break;
      }
    }
    if (chLength > 0) {
      cur = new_token(TK_IDENT, cur, p, chLength);
      p = p + chLength;
      continue;
    }


    error("cannot tokenize");
  }

  new_token(TK_EOF, cur, p,1);
  return head.next;
}

