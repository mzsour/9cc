#include "9cc.h"

extern Token *token;
extern char *user_input;

int main(int argc, char **argv){
	if(argc != 2){
		error("invalid num of args");
		return 1;
	}

	user_input = argv[1];
	// tokenize
	token = tokenize(user_input);
	Node *node = expr();

	// output header of assembly
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	gen(node);

	printf(" pop rax\n");
	printf(" ret\n");
	return 0;
}

