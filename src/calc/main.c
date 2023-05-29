#include <stdio.h>
#include "9cc.h"

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "argment is not enough\n");
    return 1;
  }

  user_input = argv[1];  
  token = tokenize(argv[1]);
  Node *node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");
  /* printf("  mov rax, %d\n", expect_number()); // pop first number */
  gen(node);

  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}



