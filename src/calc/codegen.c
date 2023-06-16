#include "9cc.h"
#include <stdio.h>


void gen_lval(Node *node){
  if (node->kind != ND_LVAR) {
    error("it is not var\n");
  }
  printf("  mov rax, rbp\n");             // base pointerを取得
  printf("  sub rax, %d\n", node->offset); // base pointer-offsetsすることで、identの値が格納されているメモリにアクセスするraxを生成
  printf("  push rax\n"); // アクセスする値をpush
  return;
}

void gen(Node *node) {

  switch (node->kind) {
  case ND_RETURN: {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  }
  case ND_NUM: {
    printf("  push %d\n", node->val); //値をpush.演算時にpopされる
    return;
  }
  case ND_LVAR: {
    gen_lval(node);
    printf("  pop rax\n");         // identにアクセスするメモリを取得
    printf("  mov rax, [rax]\n");  // メモリから値を取り出してraxに保存
    printf("  push rax\n");        // 取り出した値をraxにpush
    return;
  }
  case ND_ASSIGN: {
    gen_lval(node->lhs);  // 左辺値のメモリをrspに格納
    gen(node->rhs);       // 右辺値の計算を先行して行う

    printf("  pop rdi\n");        // 右辺値の取り出し
    printf("  pop rax\n");        // 格納するメモリを取得
    printf("  mov [rax], rdi\n"); // 左辺値のメモリ番地に右辺値を格納する
    printf("  push rdi\n");       // 右辺値をspbにpush(使う予定あるんかな。。。)
    
    return;
  }
  case ND_IF: {
    unsigned int lLabel_if = Lend_number++;
    unsigned int lLabel_else = Lend_number++;
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    if (node->els==NULL) {
      printf("  je  .Label%u\n", lLabel_if);
      gen(node->then);
      printf(".Label%u:\n", lLabel_if);
    }
    else {
      printf("  je .Label%u\n", lLabel_if);
      gen(node->then);
      printf("  jmp .Label%u\n", lLabel_else);
      printf(".Label%u:\n", lLabel_if);
      gen(node->els);
      printf(".Label%u:\n", lLabel_else);
    }
    return;
  }
  case ND_WHILE: {
    unsigned int lLabel_begin = Lend_number++;
    unsigned int lLabel_end   = Lend_number++;

    printf(".Label%u:\n", lLabel_begin);
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax,0\n");
    printf("  je .Label%u\n", lLabel_end);
    gen(node->then);
    printf("  jmp .Label%u\n", lLabel_begin);
    printf(".Label%u:\n", lLabel_end);
    return;
  }
  case ND_FOR: {
    unsigned int lLabel_begin = Lend_number++;
    unsigned int lLabel_end   = Lend_number++;
    gen(node->initial);
    printf(".Label%u:\n", lLabel_begin);
    gen(node->exitcond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Label%u\n", lLabel_end);
    gen(node->then);
    gen(node->loop);
    printf("  jmp .Label%u\n", lLabel_begin);
    printf(".Label%u:\n", lLabel_end);
    return;
  }
  case ND_BLOCK: {
    Node *ptr = node->block_next;
    while (ptr != NULL) {
      gen(ptr);
      ptr = ptr->block_next;
    }
    return;
  }
  default:
    // なにもしない
    break;
  }

  /* if (node-> kind == ND_NUM) { */
  /*   printf("  push %d\n", node->val); */
  /*   return; */
  /* } */
  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");
  switch (node->kind) {
  case ND_ADD: {
    printf("  add rax, rdi\n");
    break;
  }
  case ND_SUB: {
    printf("  sub rax, rdi\n");
    break;
  }
  case ND_MUL: {
    printf("  imul rax, rdi\n");
    break;
  }

  case ND_DIV: {
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  }

  case ND_EQ: {
    printf("  cmp  rax,rdi\n");
    printf("  sete al\n");
    break;
  }
  case ND_NE: {
    printf("  cmp   rax,rdi\n");
    printf("  setne al\n");       //
    printf("  movzb rax,al\n");
    break;
  }
  case ND_LT: {
    printf("  cmp   rax,rdi\n");
    printf("  setl  al\n");
    printf("  movzb rax,al\n");
    break;
  }
  case ND_LE: {
    printf("  cmp    rax,rdi\n");
    printf("  setle  al\n");
    printf("  movzb rax,al\n");
    break;
  }
  case ND_GT: {
    printf("  cmp   rdi,rax\n");
    printf("  setl  al\n");
    printf("  movzb rax,al\n");
    break;
  }
  case ND_GE: {
    printf("  cmp    rdi,rax\n");
    printf("  setle  al\n");
    printf("  movzb rax,al\n");
    break;
  }
    


  default:
    break;
  }
  printf("  push rax\n");
}
