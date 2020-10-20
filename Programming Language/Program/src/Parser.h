#pragma once

#include<string>
#include<stack>
#include<map>
#include<fstream>
#include"Lexer.h"

struct Parse_Node // Left-child Right-sibling Tree
{
	bool is_Terminal;
	int type;
	std::string data;
	Parse_Node* child; 
	Parse_Node* sibling;
};

namespace NT {
	enum {
		start, prog, prog_, decls, decls_, decl, words, words_, vtype, block, slist, slist_, stat, stat_, cond, cond_,
		expr, expr_, term, term_, fact
	};
}

namespace T {
	enum {
		word=21, parenO, parenC, Semicolon, Comma, INT, CHAR, BLOCKO, BLOCKC, IF, THEN, ELSE, WHILE, RETURN, ASSIGN, INEQUAL, EQUAL, ADD, MUL, NUM,	$, EPSILON
	};
}

class Parser
{
public:
	Parser();
	bool Parsing(Lexer &lexer); // Parsing ½ÃÀÛ
	void travel(Parse_Node* current);	// Parse Tree Å×½ºÆ®¿ë
	Parse_Node* get_Root(); //ParseTree Root ¹ÝÈ¯

private:
	Parse_Node* New_Node(bool is_Terminal, int type, const std::string & data); // ³ëµå »ý¼º
	void Add_child(Parse_Node* current_node, Parse_Node* child); // Child Node Ãß°¡
	void Add_sibling(Parse_Node* current_node, Parse_Node* sibling); // Sibling Node Ãß°¡
	void make_ParseTable(); // ParseTable »ý¼º
	void pushT(int l, int c, int n, ...); // ParseTable Stack¿¡ Transition ´ã±â
	void set_Index(); // Input TokenÀÇ Index È¹µæ¿ë Map »ý¼º
	void set_NT_data(); // StackÀÇ data È¹µæ¿ë Map È¹µæ
	int get_Col(Token token); // Input TokenÀÇ ParseTable Index È¹µæ

	std::stack<Parse_Node*> stack; // Parser Stack
	Token input_token;
	Parse_Node* Root;	// ParseTreeÀÇ Root³ëµå
	std::stack<int> ParseTable[21][43];
	std::map<std::string, int> C_index; // Input Token Index Map
	std::map<int, std::string> NT_data; // Stack data Map
};