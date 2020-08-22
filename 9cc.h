#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// token kind
typedef enum{
	TK_RESERVED,
	TK_RETURN,
	TK_IF,
	TK_WHILE,
	TK_FOR,
	TK_ELSE,
	TK_IDENT,
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
	int len;
};

typedef struct LVar LVar;

// local variable
struct LVar{
	LVar *next;
	char *name;
	int len; //length of name
	int offset; //offset from rbp
};

typedef enum{
	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV,
	ND_NUM,
	ND_EQ,
	ND_NE,
	ND_GT,
	ND_GE,
	ND_ASSIGN,
	ND_LVAR,
	ND_RETURN,
	ND_IF,
	ND_IFEL,
	ND_WHILE,
	ND_FOR,
	ND_NULL,
	ND_BLOCK,
	ND_FCALL,
	ND_FUNCTION,
} NodeKind;

typedef struct Node Node;

struct Node{
	NodeKind kind;
	Node *lhs;
	Node *rhs;
	Node *third_hand;
	Node *fourth_hand;
	int val; //only for ND_NUM
	int offset; //only for ND_LVAR
	char *name;
	int len;
	Node *block[100];
	Node *args[6];
};

LVar *find_lvar(Token *tok);
int is_alphabet(char c);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
bool consume(char *op);
bool consume_ctrl(int tk_type);
Token *consume_indent();
Token *expect_ident();
void expect(char *op);
int expect_number();
void expect_else();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize(char *p);
Node *primary();
Node *mul();
Node *add();
Node *relational();
Node *equality();
Node *assign();
Node *expr();
Node *stmt();
void program();
void gen(Node *node);

// current token
Token *token;
char *user_input;
Node *code[100];
LVar *locals;
int lnum;
