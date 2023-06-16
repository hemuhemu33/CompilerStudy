#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

unsigned int Lend_number = 0;

void init(){
  locals = calloc(1, sizeof(LVar));
  locals->offset = 0;
  locals->len = 0;
  locals->name = "";
  locals->next = NULL;
  Lend_number = 0;
}

int main(int argc, char *argv[])
{
  init();
  if (argc != 2) {
    fprintf(stderr, "argment is not enough\n");
    return 1;
  }

  user_input = argv[1];  
  token = tokenize(argv[1]);

  /***************************************************************************/
  /*                                  debug                                  */
  /***************************************************************************/
  /* Token *head = token; */
  /* for (; head!=NULL; head = head->next) { */
  /*   printf("token info = %d, %c, %d,%d\n",head->kind, head->str[0], head->len, head->val); */
  /* } */

  

  program();
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");
  /* printf("  mov rax, %d\n", expect_number()); // pop first number */

  // prologue
  /***************************************************************************/
  /*                      Local variable分のメモリを確保                     */
  /***************************************************************************/
  int alpha_num = 'z'-'a'+1;
  int memory_ac = alpha_num * 8;

  
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, %d\n", memory_ac);
  
  // generate assembler
  for (int i = 0; i < CODE_COLUMN; ++i) {
    if (code[i] == NULL) {
      break;
    }
    gen(code[i]);

    // 最終評価結果がメモリに残っているため、取り出す
    /* printf("  pop rax \n"); */
  }

  /* printf("  mov rsp, rbp\n"); */
  /* printf("  pop rbp\n"); */
  /* printf("  ret\n"); */
  return 0;
}



