#pragma once

#include<string>
#include<map>
#include<vector>

#include"SymbolTable.h"
#include"Parser.h"

namespace AST
{
	struct AST_Info {
		int * error;
		SymbolTable * symbol_table;
		std::vector<int> parent_id;
		int pid;
	};

	struct AST_Node
	{
		std::string data;
		int block_id;
		int type;
		int level;
		bool is_leaf;
		AST_Node * left = nullptr, *right = nullptr;
		AST_Node * T = nullptr, * F = nullptr;
		AST_Node * next = nullptr;
	};


	//Abstract Sntax Tree for Each function
	std::map<std::string, AST_Node*> getAST(Parse_Node * root, SymbolTable * symbol_table, int * error);

	void handleError(Parse_Node * node, long long & block_id);
	void showError();

	//construct tree from Non Terminals
	AST_Node * block(Parse_Node * block, AST_Node * ast, long long & block_id, unsigned int block_level);

	AST_Node * slist(Parse_Node * slist, AST_Node * ast, long long & block_id, unsigned int block_level);
	AST_Node * stat(Parse_Node * stat, AST_Node * ast, long long & block_id, unsigned int block_level);
	void expr(Parse_Node * expr, AST_Node * ast, long long & block_id);
	void cond(Parse_Node * cond, AST_Node * ast, long long & block_id);
	void term(Parse_Node * term, AST_Node * ast, long long & block_id);

	void parameter(Parse_Node * words, AST_Node * ast, long long & block_id);

	void decls(Parse_Node * declares, long long & block_id, unsigned int block_level);
	void assignVariables(Parse_Node * words, SYMBOL_TYPE type, SYMBOL_SCOPE scope, unsigned int block_level, long long & block_id);
};