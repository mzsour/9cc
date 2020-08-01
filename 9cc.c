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

typedef enum{
	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV,
	ND_NUM,
} NodeKind;

typedef struct Node Node;

struct Node{
	NodeKind kind;
	Node *lhs;
	Node *rhs;
	int val;
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
bool consume(char op);
void expect(char op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str);
Token *tokenize(char *p);
Node *primary();
Node *mul();
Node *expr();
void gen(Node *node);

Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val){
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}

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

// tokenizer
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

		if(*p=='+'||*p=='-'||*p=='*'||*p=='/'||*p=='('||*p==')'){
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

// parser
Node *primary(){
	// if next token is '(', '('expr')' should follow.
	if(consume('(')){
		Node *node = expr();
		expect(')');
		return node;
	}
	// otherwise, the token should be number.
	return new_node_num(expect_number());
}

Node *mul(){
	Node *node = primary();

	for(;;){
		if(consume('*'))
			node = new_node(ND_MUL, node, primary());
		else if(consume('/'))
			node = new_node(ND_DIV, node, primary());
		else
			return node;
	}
}

Node *expr(){
	Node *node = mul();

	for(;;){
		if(consume('+'))
			node = new_node(ND_ADD, node, mul());
		else if(consume('-'))
			node = new_node(ND_SUB, node, mul());
		else
			return node;
	}
}

// generator
void gen(Node *node){
	if(node->kind == ND_NUM){
		printf(" push %d\n", node->val);
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf(" pop rdi\n");
	printf(" pop rax\n");

	switch(node->kind){
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
		



