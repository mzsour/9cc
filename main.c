#include "9cc.h"

extern Token *token;
extern char *user_input;
extern Node *code[100];

int main(int argc, char **argv){
	if(argc != 2){
		error("invalid num of args");
		return 1;
	}

	user_input = argv[1];
	// tokenize
	token = tokenize(user_input);
	program();

	// output header of assembly
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	for(int i=0; code[i]; i++){
		gen(code[i]);

		printf(" pop rax\n");
	}

	printf(" ret\n");
	return 0;
}

