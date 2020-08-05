#include "9cc.h"

extern Token *token;
extern char *user_input;
extern Node *code[100];
extern LVar *locals;

int main(int argc, char **argv){
	if(argc != 2){
		error("invalid num of args");
		return 1;
	}

	user_input = argv[1];
	token = tokenize(user_input);
	locals = NULL;

	program();
	
	// header of assembly
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	// prologue
	printf(" push rbp\n");
	printf(" mov rbp, rsp\n");
	printf(" sub rsp, 208\n");

	for(int i=0; code[i]; i++){
		gen(code[i]);

		printf(" pop rax\n");
	}

	//epilogue
	printf(" mov rsp, rbp\n");
	printf(" pop rbp\n");
	printf(" ret\n");
	return 0;
}

