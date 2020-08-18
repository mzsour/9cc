#include "9cc.h"

extern Token *token;
extern char *user_input;
extern Node *code[100];
extern LVar *locals;
extern int lnum;

int main(int argc, char **argv){
	if(argc != 2){
		error("invalid num of args");
		return 1;
	}

	user_input = argv[1];
	token = tokenize(user_input);
	locals = NULL;
	lnum = 0;

	program();
	
	// header of assembly
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");

	for(int i=0; code[i]; i++){
		gen(code[i]);
	}

	return 0;
}

