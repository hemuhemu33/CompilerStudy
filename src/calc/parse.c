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



Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
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
  Node *node = equality();
  return node;
}


Node *primary(){
  if (consume("(")) {
    Node *node = expr();
    expect(')');
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



Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
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
	|| *p == '<' || *p == '>') {
      cur = new_token(TK_RESERVED, cur, p++,1);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p,-1); // これは何でもいい。
      cur->val = strtol(p, &p, 10);
      continue;
    }
    error("cannot tokenize");
  }

  new_token(TK_EOF, cur, p,1);
  return head.next;
}

