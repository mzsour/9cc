#include "9cc.h"

int is_alphabet(char c){
	return('a' <= c && c <= 'z') ||
		('A' <= c && c <= 'Z') ||
		(c == '_');
}

int is_alnum(char c){
	return('a' <= c && c <= 'z') ||
		('A' <= c && c <= 'Z') ||
		('0' <= c && c <= '9') ||
		(c == '_');
}

LVar *find_lvar(Token *tok){
	for(LVar *var = locals; var; var = var->next){
		if(var->len == tok->len && !memcmp(tok->str, var->name, var->len)){
			return var;
		}
	}
	return NULL;
}

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

bool search_for_token(int type){
	Token *t;
	for(t=token; t->kind != TK_EOF; t=t->next){
		if(t->kind == type){
			return true;
		}
	}
	return false;
}

bool consume_ctrl(int tk_type){
	if(token->kind != tk_type)
		return false;
	token = token->next;
	return true;
}

Token *consume_ident(){
	if(token->kind != TK_IDENT)
		return NULL;
	Token *res = token;
	token = token->next;
	return res;
}

void expect(char *op){
	if(token->kind != TK_RESERVED ||
		strlen(op) != token->len ||
		memcmp(token->str, op, token->len))
		error_at(token->str, "not %s", op);
	token = token->next;
}

void expect_else(){
	if(token->kind != TK_ELSE){
		error_at(token->str, "not else");
	}
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

		if(strncmp(p, "return", 6) == 0 && !is_alnum(p[6])){
			cur = new_token(TK_RETURN, cur, p, 6);
			p += 6;
			continue;
		}

		if(strncmp(p, "if", 2) == 0 && !is_alnum(p[2])){
			cur = new_token(TK_IF, cur, p, 2);
			p += 2;
			continue;
		}

		if(strncmp(p, "else", 4) == 0 && !is_alnum(p[4])){
			cur = new_token(TK_ELSE, cur, p, 4);
			p += 4;
			continue;
		}

		if(strncmp(p, "while", 5) == 0 && !is_alnum(p[5])){
			cur = new_token(TK_WHILE, cur, p, 5);
			p += 5;
			continue;
		}

		if(strncmp(p, "for", 3) == 0 && !is_alnum(p[3])){
			cur = new_token(TK_FOR, cur, p, 3);
			p += 3;
			continue;
		}


		if(is_alphabet(*p)){
			int len = 0;
			while(is_alphabet(*p)){
				len++;
				p++;
			}
			cur = new_token(TK_IDENT, cur, p - len, len);
			continue;
		}

		if(strncmp(p, "==", 2) == 0 ||
			strncmp(p, "!=", 2) == 0 ||
			strncmp(p, ">=", 2) == 0 ||
			strncmp(p, "<=", 2) == 0){
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}

		if(*p=='+'||*p=='-'||*p=='*'||*p=='/'||*p=='>'||
			*p=='<'||*p=='('||*p==')'||*p==';'||*p=='='){
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
	Token *tok = consume_ident();
	if(tok){
		Node *node = calloc(1, sizeof(Node));
		node->kind = ND_LVAR;

		LVar *lvar = find_lvar(tok);
		if(lvar){
			node->offset = lvar->offset;
		} else {
			lvar = calloc(1, sizeof(LVar));
			lvar->next = locals;
			lvar->name = tok->str;
			lvar->len = tok->len;
			if(!locals){
				lvar->offset = 8;
			} else {
				lvar->offset = locals->offset + 8;
			}
			node->offset = lvar->offset;
			locals = lvar;
		}
		return node;
	}

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

Node *assign(){
	Node *node = equality();
	if(consume("="))
		node = new_node(ND_ASSIGN, node, assign());
	return node;
}

Node *expr(){
	Node *node;

	node = assign();
	return node;
}

Node *stmt(){
	Node *node;

	if(consume_ctrl(TK_RETURN)){
		node = calloc(1, sizeof(Node));
		node->kind = ND_RETURN;
		node->lhs = expr();
	} else if(consume_ctrl(TK_IF)){
		node = calloc(1, sizeof(Node));
		if(search_for_token(TK_ELSE)){
			node->kind = ND_IFEL;
		} else {
			node->kind = ND_IF;
		}
		expect("(");
		node->lhs = expr();
		expect(")");
		node->rhs = stmt();
		if(node->kind == ND_IFEL){
			expect_else();
			node->third_hand = stmt();
		}
	} else if(consume_ctrl(TK_WHILE)){
		node = calloc(1, sizeof(Node));
		node->kind = ND_WHILE;
		expect("(");
		node->lhs = expr();
		expect(")");
		node->rhs = stmt();
	} else if(consume_ctrl(TK_FOR)){
		node = calloc(1, sizeof(Node));
		node->kind = ND_FOR;
		expect("(");
		if(consume(";")){
			node->lhs = new_node(ND_NULL, NULL, NULL);
		} else {
			node->lhs = expr();
			expect(";");
		}
		if(consume(";")){
			node->rhs = new_node(ND_NULL, NULL, NULL);
		} else {
			node->rhs = expr();
			expect(";");
		}
		if(consume(")")){
			node->third_hand = new_node(ND_NULL, NULL, NULL);
		} else {
			node->third_hand = expr();
			expect(")");
		}
		node->fourth_hand = stmt();
	} else {
		node = expr();
	}

	return node;
}

void program(){
	int i = 0;
	while(!at_eof()){
		Node *node;
		node = stmt();
		expect(";");
		code[i++] = node;
	}
	code[i] = NULL;
}

