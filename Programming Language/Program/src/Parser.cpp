#include"Parser.h"
#include <stdarg.h>
#include <iostream>

Parser::Parser()
{
	make_ParseTable();
	Root = New_Node(false, NT::start, "start");
	stack.push(Root);
	set_Index(); 
	set_NT_data();
	//Ostream(); //테스트 출력용
}

void Parser::pushT(int l, int c, int n, ...)
{
	va_list t;
	va_start(t, n);
	for (int i = 0; i < n; i++) {
		ParseTable[l][c].push(va_arg(t, int));
	}
	va_end(t);
}

void Parser::make_ParseTable()
{
	pushT(NT::start, T::INT, 2, NT::prog, NT::prog_); pushT(NT::start, T::CHAR, 2, NT::prog, NT::prog_);
	pushT(NT::prog, T::INT, 6, NT::vtype, T::word, T::parenO, NT::words, T::parenC, NT::block); pushT(NT::prog, T::CHAR, 6, NT::vtype, T::word, T::parenO, NT::words, T::parenC, NT::block);
	pushT(NT::prog_, T::INT, 7, NT::vtype, T::word, T::parenO, NT::words, T::parenC, NT::block, NT::prog_); pushT(NT::prog_, T::CHAR, 7, NT::vtype, T::word, T::parenO, NT::words, T::parenC, NT::block, NT::prog_); pushT(NT::prog_, T::$, 1, T::EPSILON);
	pushT(NT::decls, T::word, 1, NT::decls_); pushT(NT::decls, T::INT, 1, NT::decls_); pushT(NT::decls, T::CHAR, 1, NT::decls_); pushT(NT::decls, T::IF, 1, NT::decls_); pushT(NT::decls, T::WHILE, 1, NT::decls_); pushT(NT::decls, T::RETURN, 1, NT::decls_);
	pushT(NT::decls_, T::word, 1, T::EPSILON); pushT(NT::decls_, T::INT, 2, NT::decl, NT::decls_); pushT(NT::decls_, T::CHAR, 2, NT::decl, NT::decls_); pushT(NT::decls_, T::IF, 1, T::EPSILON); pushT(NT::decls_, T::WHILE, 1, T::EPSILON); pushT(NT::decls_, T::RETURN, 1, T::EPSILON);
	pushT(NT::decl, T::INT, 3, NT::vtype, NT::words, T::Semicolon); pushT(NT::decl, T::CHAR, 3, NT::vtype, NT::words, T::Semicolon);
	pushT(NT::words, T::word, 2, T::word, NT::words_);
	pushT(NT::words_, T::parenC, 1, T::EPSILON); pushT(NT::words_, T::Semicolon, 1, T::EPSILON); pushT(NT::words_, T::Comma, 3, T::Comma, T::word, NT::words_);
	pushT(NT::vtype, T::INT, 1, T::INT); pushT(NT::vtype, T::CHAR, 1, T::CHAR);
	pushT(NT::block, T::BLOCKO, 4, T::BLOCKO, NT::decls, NT::slist, T::BLOCKC);
	pushT(NT::slist, T::word, 2, NT::stat, NT::slist_); pushT(NT::slist, T::IF, 2, NT::stat, NT::slist_); pushT(NT::slist, T::WHILE, 2, NT::stat, NT::slist_); pushT(NT::slist, T::RETURN, 2, NT::stat, NT::slist_);
	pushT(NT::slist_, T::word, 2, NT::stat, NT::slist_); pushT(NT::slist_, T::BLOCKC, 1, T::EPSILON); pushT(NT::slist_, T::IF, 2, NT::stat, NT::slist_); pushT(NT::slist_, T::WHILE, 2, NT::stat, NT::slist_); pushT(NT::slist_, T::RETURN, 2, NT::stat, NT::slist_);
	pushT(NT::stat, T::word, 2, T::word, NT::stat_); pushT(NT::stat, T::IF, 6, T::IF, NT::cond, T::THEN, NT::block, T::ELSE, NT::block); pushT(NT::stat, T::WHILE, 3, T::WHILE, NT::cond, NT::block); pushT(NT::stat, T::RETURN, 3, T::RETURN, NT::expr, T::Semicolon);
	pushT(NT::stat_, T::parenO, 4, T::parenO, NT::words, T::parenC, T::Semicolon); pushT(NT::stat_, T::ASSIGN, 3, T::ASSIGN, NT::expr, T::Semicolon);
	pushT(NT::cond, T::word, 2, NT::expr, NT::cond_); pushT(NT::cond, T::NUM, 2, NT::expr, NT::cond_);
	pushT(NT::cond_, T::INEQUAL, 2, T::INEQUAL, NT::expr); pushT(NT::cond_, T::EQUAL, 2, T::EQUAL, NT::expr);
	pushT(NT::expr, T::word, 2, NT::term, NT::expr_); pushT(NT::expr, T::NUM, 2, NT::term, NT::expr_);
	pushT(NT::expr_, T::Semicolon, 1, T::EPSILON); pushT(NT::expr_, T::BLOCKO, 1, T::EPSILON); pushT(NT::expr_, T::THEN, 1, T::EPSILON); pushT(NT::expr_, T::INEQUAL, 1, T::EPSILON); pushT(NT::expr_, T::EQUAL, 1, T::EPSILON); pushT(NT::expr_, T::ADD, 2, T::ADD, NT::term);
	pushT(NT::term, T::word, 2, NT::fact, NT::term_); pushT(NT::term, T::NUM, 2, NT::fact, NT::term_);
	pushT(NT::term_, T::Semicolon, 1, T::EPSILON); pushT(NT::term_, T::BLOCKO, 1, T::EPSILON); pushT(NT::term_, T::THEN, 1, T::EPSILON); pushT(NT::term_, T::INEQUAL, 1, T::EPSILON); pushT(NT::term_, T::EQUAL, 1, T::EPSILON); pushT(NT::term_, T::ADD, 1, T::EPSILON); pushT(NT::term_, T::MUL, 2, T::MUL, NT::fact);
	pushT(NT::fact, T::word, 1, T::word); pushT(NT::fact, T::NUM, 1, T::NUM);
}

void Parser::set_Index()
{
	C_index["("] = T::parenO;
	C_index[")"] = T::parenC;
	C_index[";"] = T::Semicolon;
	C_index[","] = T::Comma;
	C_index["int"] = T::INT;
	C_index["char"] = T::CHAR;
	C_index["{"] = T::BLOCKO;
	C_index["}"] = T::BLOCKC;
	C_index["IF"] = T::IF;
	C_index["THEN"] = T::THEN;
	C_index["ELSE"] = T::ELSE;
	C_index["WHILE"] = T::WHILE;
	C_index["RETURN"] = T::RETURN;
	C_index["="] = T::ASSIGN;
	C_index[">"] = T::INEQUAL;
	C_index["=="] = T::EQUAL;
	C_index["+"] = T::ADD;
	C_index["*"] = T::MUL;
	C_index["$"] = T::$;
}

void Parser::set_NT_data()
{
	NT_data[NT::start] = "start";
	NT_data[NT::prog] = "prog";
	NT_data[NT::prog_] = "prog_";
	NT_data[NT::decls] = "decls";
	NT_data[NT::decls_] = "decls_";
	NT_data[NT::decl] = "decl";
	NT_data[NT::words] = "words";
	NT_data[NT::words_] = "words_";
	NT_data[NT::vtype] = "vtype";
	NT_data[NT::block] = "block";
	NT_data[NT::slist] = "slist";
	NT_data[NT::slist_] = "slist_";
	NT_data[NT::stat] = "stat";
	NT_data[NT::stat_] = "stat_";
	NT_data[NT::cond] = "cond";
	NT_data[NT::cond_] = "cond_";
	NT_data[NT::expr] = "expr";
	NT_data[NT::expr_] = "expr_";
	NT_data[NT::term] = "term";
	NT_data[NT::term_] = "term_";
	NT_data[NT::fact] = "fact";
}

int Parser::get_Col(Token token)
{
	if (token.type == TOKEN_TYPE::END)return T::$;
	else if (token.type == TOKEN_TYPE::LITERAL)return T::NUM;
	else if (token.type == TOKEN_TYPE::IDENTIFIER)return T::word;
	else return C_index[token.data];
}

bool Parser::Parsing(Lexer & lexer)
{
	Token input = lexer.getNextToken();
	Parse_Node* Current;
	Parse_Node* nd;
	std::stack<Parse_Node*> tmp;	//stack for reverse Tree
	std::stack<int> tstack;
	int row, col, top;
	while (!stack.empty()) {
		Current = stack.top();
		stack.pop();
		if (Current->type == T::EPSILON) { // EPSILON 처리
			Current->data = "EPSILON";
		}
		else if (Current->is_Terminal) { // Input Pop
			Current->data = input.data;
			input = lexer.getNextToken();
		}
		else // Transition 진행
		{
			row = Current->type;
			col = get_Col(input);
			tstack = ParseTable[row][col];
			if (tstack.empty()) {
				std::cout << "Syntax Error: Parser Rejected!" << std::endl;
				return true;
			}
			while (!tstack.empty()) {
				top = tstack.top();
				nd = New_Node((top >= 21 ? true : false), top, NT_data[top]);
				stack.push(nd);
				tmp.push(nd);
				tstack.pop();
			}
			while (!tmp.empty()) {	// reverse Tree
				Add_child(Current, tmp.top());
				tmp.pop();
			}
		}
	}
	return false;
}

Parse_Node* Parser::get_Root()
{
	return Root;
}

void Parser::travel(Parse_Node* current)
{
	if (!current->data.compare("{"))std::cout << std::endl;
	std::cout << current->data << " ";
	if (!current->data.compare(";") || !current->data.compare("}"))std::cout << std::endl;
	if (current->child) {
		travel(current->child);
	}
	if (current->sibling) {
		travel(current->sibling);
	}
}
Parse_Node* Parser::New_Node(bool is_Terminal, int type, const std::string & data)
{
	Parse_Node* nd = new Parse_Node;
	nd->sibling = nd->child = NULL;
	nd->data = data;
	nd->is_Terminal = is_Terminal;
	nd->type = type;
	return nd;
}

void Parser::Add_child(Parse_Node* current_node, Parse_Node* child)
{
	if (current_node->child) Add_sibling(current_node->child, child);
	else current_node->child = child;
}

void Parser::Add_sibling(Parse_Node* current_node, Parse_Node* sibling)
{
	while (current_node->sibling) current_node = current_node->sibling;
	current_node->sibling = sibling;
}
