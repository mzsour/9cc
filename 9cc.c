#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// token kind
typedef enum{
	TK_RESERVED,
	TK_NUM,
	TK_EOF,
} TokenKind;

typedef struct Token Token;

// token structure
struct Token{
	TokenKind kind;
	Token *next;
	int val;
	char *str;
};

// current token
Token *token;

char *user_input;

void error_at(char *loc, char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, "");
	fprintf(stderr, "^");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}


// error reporting function
void error(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

// if the next token is reserved one,
// read one step ahead and return true.
// else return false.
bool consume(char op){
	if(token->kind != TK_RESERVED || token->str[0] != op)
		return false;
	token = token->next;
	return true;
}

void expect(char op){
	if(token->kind != TK_RESERVED || token->str[0] != op)
		error_at(token->str, "not %c", op);
	token = token->next;
}

// if the next token is not a number, report error.
// if it is a number, read ahead token
// and return the value.
int expect_number(){
	if(token->kind != TK_NUM)
		error_at(token->str, "not number");
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof(){
	return token->kind == TK_EOF;
}

// create new token and connect it to cur
Token *new_token(TokenKind kind, Token *cur, char *str){
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return tok;
}

// tokenize string p and return it
Token *tokenize(char *p){
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while(*p){
		// skip white spaces
		if(isspace(*p)){
			p++;
			continue;
		}

		if(*p == '+' || *p == '-'){
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}

		if(isdigit(*p)){
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error("failed to tokenize");
	}

	new_token(TK_EOF, cur, p);
	return head.next;
}

int main(int argc, char **argv){
	user_input = argv[1];
	if(argc != 2){
		error("invalid num of args");
		return 1;
	}

	// tokenize
	token = tokenize(argv[1]);

	// output header of assembly
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	// check that first token is number
	// output mov operation with the number
	printf(" mov rax, %d\n", expect_number());

	//
	while(!at_eof()){
		if(consume('+')){
			printf(" add rax, %d\n", expect_number());
			continue;
		}

		expect('-');
		printf(" sub rax, %d\n", expect_number());
	}

	printf(" ret\n");
	return 0;
}
		



