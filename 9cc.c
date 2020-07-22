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
	int val; // if token kind is number
	char *str;
};

// current token
Token *token;

void error(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

// if the next token is reserved operator,
// read forward next token and return true.
// otherwise return false.
bool consume(char op){
	if(token->kind != TK_RESERVED || token->str[0] != op)
		return false;
	token = token->next;
	return true;
}

// if the next token is expected operator,
// read forward next token. otherwise output error.
void expect(char op){
	if(token->kind != TK_RESERVED || token->str[0] != op)
		error("is not '%c'", op);
	token = token->next;
}

// if the next token is number, read forward next token
// and return the value of the number. otherwise output error.
int expect_number(){
	if(token->kind != TK_NUM)
		error("is not number");
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

// tokenize string p and return the resulted token list
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
	if(argc != 2){
		error("invalid num of args");
		return 1;
	}

	token = tokenize(argv[1]);

	// output header of assembly
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");
	
	// expression must start with number.
	// check that and output first mov
	printf("  mov rax, %d\n", expect_number());

	// consuming token of `+ num` or `- num`
	// and output assembly
	while(!at_eof()){
		if(consume('+')){
			printf("  add rax, %d\n", expect_number());
			continue;
		}

		expect('-');
		printf("  sub rax, %d\n", expect_number());
	}

	printf("  ret\n");
	return 0;
}





