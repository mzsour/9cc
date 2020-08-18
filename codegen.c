#include "9cc.h"

// evaluate variable as left side value.
// memory address of the variable is pushed.
void gen_lval(Node *node){
	if(node->kind != ND_LVAR)
		error("left side value is not variable");

	printf(" mov rax, rbp\n");
	printf(" sub rax, %d\n", node->offset);
	printf(" push rax\n");
}

// generator
void gen(Node *node){
	int i = 0;
	switch(node->kind){
	case ND_NUM:
		printf(" push %d\n", node->val);
		return;

	// evaluate variable as right side value.
	// the value of the variable is pushed.
	case ND_LVAR:
		gen_lval(node);
		printf(" pop rax\n");
		printf(" mov rax, [rax]\n");
		printf(" push rax\n");
		return;

	case ND_ASSIGN:
		gen_lval(node->lhs);
		gen(node->rhs);

		printf(" pop rdi\n");
		printf(" pop rax\n");
		printf(" mov [rax], rdi\n");
		printf(" push rdi\n");
		return;

	case ND_RETURN:
		gen(node->lhs);
		printf(" pop rax\n");
		printf(" mov rsp, rbp\n");
		printf(" pop rbp\n");
		printf(" ret\n");
		return;

	case ND_IF:
		gen(node->lhs);
		printf(" pop rax\n");
		printf(" cmp rax, 0\n");
		printf(" je .Lend%d\n", lnum);
		gen(node->rhs);
		printf(".Lend%d:\n", lnum++);
		return;

	case ND_IFEL:
		gen(node->lhs);
		printf(" pop rax\n");
		printf(" cmp rax, 0\n");
		printf(" je .Lelse%d\n", lnum);
		gen(node->rhs);
		printf(" jmp .Lend%d\n", lnum);
		printf(".Lelse%d:\n", lnum);
		gen(node->third_hand);
		printf(".Lend%d:\n", lnum);
		lnum++;
		return;

	case ND_WHILE:
		printf(".Lbegin%d:\n", lnum);
		gen(node->lhs);
		printf(" pop rax\n");
		printf(" cmp rax, 0\n");
		printf(" je .Lend%d\n", lnum);
		gen(node->rhs);
		printf(" jmp .Lbegin%d\n", lnum);
		printf(".Lend%d:\n", lnum);
		lnum++;
		return;

	case ND_FOR:
		gen(node->lhs);
		printf(".Lbegin%d:\n", lnum);
		gen(node->rhs);
		printf(" pop rax\n");
		printf(" cmp rax, 0\n");
		printf(" je .Lend%d\n", lnum);
		gen(node->fourth_hand);
		gen(node->third_hand);
		printf(" jmp .Lbegin%d\n", lnum);
		printf(".Lend%d:\n", lnum);
		lnum++;
		return;

	case ND_BLOCK:
		while(node->block[i] != NULL){
			gen(node->block[i]);
			//printf(" pop rax\n");
			i++;
		}
		return;

	case ND_FCALL:
		if(node->args[0] != NULL){
			gen(node->args[0]);
			printf(" pop rax\n");
			printf(" mov rdi, rax\n");
		}
		if(node->args[1] != NULL){
			gen(node->args[1]);
			printf(" pop rax\n");
			printf(" mov rsi, rax\n");
		}
		if(node->args[2] != NULL){
			gen(node->args[2]);
			printf(" pop rax\n");
			printf(" mov rdx, rax\n");
		}
		if(node->args[3] != NULL){
			gen(node->args[3]);
			printf(" pop rax\n");
			printf(" mov rcx, rax\n");
		}
		if(node->args[4] != NULL){
			gen(node->args[4]);
			printf(" pop rax\n");
			printf(" mov r8, rax\n");
		}
		if(node->args[5] != NULL){
			gen(node->args[5]);
			printf(" pop rax\n");
			printf(" mov r9, rax\n");
		}

		printf(" call %.*s\n", node->len, node->name);
		return;

	case ND_FUNCTION:
		printf("%.*s:\n", node->len, node->name);

		// prologue
		printf(" push rbp\n");
		printf(" mov rbp, rsp\n");
		printf(" sub rsp, 208\n");

		while(node->block[i] != NULL){
			gen(node->block[i]);
			//printf(" pop rax\n");
			i++;
		}

		//epilogue
		printf(" pop rax\n");
		printf(" mov rsp, rbp\n");
		printf(" pop rbp\n");
		printf(" ret\n");
		return;


	case ND_NULL:
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf(" pop rdi\n");
	printf(" pop rax\n");

	switch(node->kind){
	case ND_EQ:
		printf(" cmp rax, rdi\n");
		printf(" sete al\n");
		printf(" movzb rax, al\n");
		break;
	case ND_NE:
		printf(" cmp rax, rdi\n");
		printf(" setne al\n");
		printf(" movzb rax, al\n");
		break;
	case ND_GT:
		printf(" cmp rax, rdi\n");
		printf(" setg al\n");
		printf(" movzb rax, al\n");
		break;
	case ND_GE:
		printf(" cmp rax, rdi\n");
		printf(" setge al\n");
		printf(" movzb rax, al\n");
		break;
	case ND_ADD:
		printf(" add rax, rdi\n");
		break;
	case ND_SUB:
		printf(" sub rax, rdi\n");
		break;
	case ND_MUL:
		printf(" imul rax, rdi\n");
		break;
	case ND_DIV:
		printf(" cqo\n");
		printf(" idiv rdi\n");
		break;
	}
	printf(" push rax\n");
}
