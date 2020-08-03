#include "9cc.h"

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
bool consume(char *op){
	if(token->kind != TK_RESERVED ||
		strlen(op) != token->len ||
		memcmp(token->str, op, token->len))
		return false;
	token = token->next;
	return true;
}

void expect(char *op){
	if(token->kind != TK_RESERVED ||
		strlen(op) != token->len ||
		memcmp(token->str, op, token->len))
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
Token *new_token(TokenKind kind, Token *cur, char *str, int len){
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	tok->len = len;
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

		if(strncmp(p, "==", 2) == 0 ||
			strncmp(p, "!=", 2) == 0 ||
			strncmp(p, ">=", 2) == 0 ||
			strncmp(p, "<=", 2) == 0){
			cur = new_token(TK_RESERVED, cur, p, 2);
			p = p + 2;
			continue;
		}

		if(*p=='+'||*p=='-'||*p=='*'||*p=='/'||*p=='>'||*p=='<'||*p=='('||*p==')'||*p==';'){
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		if(isdigit(*p)){
			cur = new_token(TK_NUM, cur, p, 0);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error("failed to tokenize");
	}

	new_token(TK_EOF, cur, p, 0);
	return head.next;
}


// parser
Node *primary(){
	// if next token is '(', '('expr')' should follow.
	if(consume("(")){
		Node *node = expr();
		expect(")");
		return node;
	}
	// otherwise, the token should be number.
	return new_node_num(expect_number());
}

Node *unary(){
	if(consume("+"))
		return primary();
	if(consume("-"))
		return new_node(ND_SUB, new_node_num(0), primary());
	return primary();
}

Node *mul(){
	Node *node = unary();

	for(;;){
		if(consume("*"))
			node = new_node(ND_MUL, node, unary());
		else if(consume("/"))
			node = new_node(ND_DIV, node, unary());
		else
			return node;
	}
}

Node *add(){
	Node *node = mul();

	for(;;){
		if(consume("+"))
			node = new_node(ND_ADD, node, mul());
		else if(consume("-"))
			node = new_node(ND_SUB, node, mul());
		else
			return node;
	}
}

Node *relational(){
	Node *node = add();

	for(;;){
		if(consume(">"))
			node = new_node(ND_GT, node, add());
		else if(consume("<"))
			// use GT and switch left and right.
			node = new_node(ND_GT, add(), node);
		else if(consume(">="))
			node = new_node(ND_GE, node, add());
		else if(consume("<="))
			node = new_node(ND_GE, add(), node);
		else
			return node;
	}
}

Node *equality(){
	Node *node = relational();
	for(;;){
		if(consume("=="))
			node = new_node(ND_EQ, node, relational());
		else if(consume("!="))
			node = new_node(ND_NE, node, relational());
		else
			return node;
	}
}

Node *expr(){
	Node *node = equality();
	return node;
}

Node *stmt(){
	Node *node = expr();
	expect(";");
	return node;
}

void program(){
	int i = 0;
	while(!at_eof())
		code[i++] = stmt();
	code[i] = NULL;
}

