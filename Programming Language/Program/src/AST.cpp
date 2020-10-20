#include "AST.h"

namespace AST
{
	AST_Info info;

	std::map<std::string, AST_Node*> getAST(Parse_Node * root, SymbolTable * symbol_table, int * error)
	{
		Parse_Node * func_node = root->child;

		std::map<std::string , AST_Node*> ast_map;

		long long block_id = 0;

		bool start = true;

		*error = 0;
		info.error = error;
		info.symbol_table = symbol_table;

		while (func_node->child->type != T::EPSILON)
		{
			AST_Node *ast = new AST_Node;

			Parse_Node * child = func_node->child;

			//push function to symbol_table
			SYMBOL_TYPE symbol_type = child->child->type == T::INT ? SYMBOL_TYPE::FUNCTION_INT : SYMBOL_TYPE::FUNCTION_CHAR;
			unsigned int data_size = child->child->type == T::INT ? 4 : 1;
			child = child->sibling;
			std::string symbol_name = child->data;

			info.parent_id.push_back(block_id);
			info.pid = block_id;
			
			if (symbol_table->getData(symbol_name, 0)) {
				*info.error = *info.error | 0x4;
			}
			symbol_table->insertData(symbol_name, 0, new SymbolData(symbol_type, SYMBOL_SCOPE::GLOBAL, 0, data_size));

			//insert ast
			ast_map.insert({ symbol_name ,ast });

			//push parameter to symbol_table
			child = child->sibling->sibling;

			// child :parameter -> words
			assignVariables(child, SYMBOL_TYPE::INT, SYMBOL_SCOPE::FUNTION_PARAMETER, 1, block_id);

			//block
			auto block_ = child = child->sibling->sibling;
			block(block_, ast, block_id, 1);
			
			if (start) {
				func_node = func_node->sibling;
				start = false;
			}
			else
				func_node = child->sibling;
			block_id++;
		}

		showError();

		return ast_map;
	}

	AST_Node * block(Parse_Node * block, AST_Node * ast, long long & block_id, unsigned int block_level)
	{
		auto decls_ = block->child->sibling;
		auto slist_ = decls_->sibling;

		decls(decls_, block_id , block_level);
		return slist(slist_, ast, block_id, block_level);
	}

	AST_Node * slist(Parse_Node * slist, AST_Node * ast, long long & block_id, unsigned int block_level)
	{
		auto stat_ = slist->child;
		auto slist_ = stat_->sibling;

		auto last_ast = stat(stat_, ast, block_id, block_level);
	
		if (slist_->child->type != T::EPSILON)
		{
			return AST::slist(slist_, last_ast->next = new AST_Node, block_id, block_level);
		}
		return last_ast->next;
	}

	AST_Node * stat(Parse_Node * stat, AST_Node * ast, long long & block_id, unsigned int block_level)
	{
		auto which = stat->child;
		AST_Node * last_node = ast;
		if (which->type == T::IF)
		{
			ast->data = which->data;
			ast->type = which->type;
			ast->is_leaf = false;
			ast->block_id = ++block_id;

			info.parent_id.push_back(info.pid);
			info.parent_id.push_back(info.pid);

			auto condition = which->sibling;

			cond(condition, ast->left = new AST_Node, block_id);

			//IF
			auto block_ = condition->sibling->sibling;
			block(block_, ast->T = new AST_Node, block_id, block_level + 1);

			//ELSE
			block_ = block_->sibling->sibling;
			block(block_, ast->F = new AST_Node, ++block_id, block_level + 1);
		}
		else if (which->type == T::WHILE)
		{
			ast->data = which->data;
			ast->type = which->type;
			ast->is_leaf = false;
			ast->block_id = ++block_id;

			info.parent_id.push_back(info.pid);

			auto condition = which->sibling;

			cond(condition, ast->left = new AST_Node, block_id);

			auto block_ = condition->sibling;
			//WHILE
			block(block_, ast->T = new AST_Node, block_id, block_level + 1);
		}
		else if (which->type == T::word)
		{
			auto op = which->sibling->child;
			if (op->type == T::ASSIGN)
			{
				ast->data = op->data;
				ast->is_leaf = false;
				ast->block_id = block_id;
				ast->type = op->type;

				ast->left = new AST_Node;
				ast->left->block_id = block_id;
				ast->left->data = which->data;
				ast->left->is_leaf = true;
				ast->left->type = which->type;

				handleError(which, block_id);

				auto expr_ = op->sibling;
				expr(expr_, ast->right = new AST_Node, block_id);
			}
			else if (op->type == T::parenO)
			{
				ast->block_id = 0;
				ast->data = which->data;
				ast->is_leaf = false;
				ast->type = T::$;

				handleError(which, block_id);

				parameter(op->sibling, ast->left = new AST_Node, block_id);
			}
		}
		else if (which->type == T::RETURN)
		{
			ast->data = which->data;
			ast->block_id = block_id;
			ast->is_leaf = false;
			ast->type = which->type;

			auto _expr = which->sibling;
			expr(_expr, ast->left = new AST_Node, block_id);
		}
		return last_node;
	}

	void parameter(Parse_Node * words, AST_Node * ast, long long & block_id)
	{
		auto word = words->child;

		ast->data = word->data;
		ast->type = word->type;
		ast->is_leaf = false;
		ast->block_id = block_id;

		auto words_ = word->sibling;
		while (words_->child->type != T::EPSILON)
		{
			word = words_->child->sibling;

			ast = ast->left = new AST_Node;
			ast->data = word->data;
			ast->type = word->type;
			ast->is_leaf = false;
			ast->block_id = block_id;

			words_ = word->sibling;
		}
		ast->is_leaf = true;
	}

	void cond(Parse_Node * cond, AST_Node * ast, long long & block_id)
	{
		auto expr1 = cond->child;
		
		expr(expr1, ast->left = new AST_Node, block_id);
		
		auto condition = expr1->sibling->child;
		
		ast->data = condition->data;
		ast->is_leaf = false;
		ast->type = condition->type;
		ast->block_id = block_id;

		auto expr2 = condition->sibling;

		expr(expr2, ast->right = new AST_Node, block_id);
	}

	void expr(Parse_Node * expr, AST_Node * ast, long long & block_id)
	{
		auto _term = expr->child;
		auto expr_ = _term->sibling;

		if (expr_->child->type == T::EPSILON)
		{
			term(_term, ast, block_id);
		}
		else
		{
			ast->data = "+";
			ast->is_leaf = false;
			ast->block_id = block_id;
			ast->type = T::ADD;

			term(_term, ast->left = new AST_Node, block_id);
			_term = expr_->child->sibling;
			term(_term, ast->right = new AST_Node, block_id);
		}
	}

	void term(Parse_Node * term, AST_Node * ast, long long & block_id)
	{
		auto fact = term->child;
		auto term_ = fact->sibling;

		if (term_->child->type == T::EPSILON)
		{
			ast->data = fact->child->data.data();
			ast->is_leaf = true;
			ast->block_id = block_id;
			ast->type = fact->child->type;
			handleError(fact->child, block_id);
		}
		else
		{
			ast->data = "*";
			ast->is_leaf = false;
			ast->block_id = block_id;
			ast->type = T::MUL;

			auto left = ast->left = new AST_Node;
			left->data = fact->child->data;
			left->block_id = block_id;
			left->is_leaf = true;
			left->type = fact->child->type;

			handleError(fact->child, block_id);

			fact = term_->child->sibling;
			auto right = ast->right = new AST_Node;
			right->data = fact->child->data;
			right->block_id = block_id;
			right->is_leaf = true;
			right->type = fact->child->type;
			
			handleError(fact->child, block_id);
		}
	}

	void decls(Parse_Node * declares, long long & block_id, unsigned int block_level)
	{
		auto decls_ = declares->child;

		while (decls_->child->type != T::EPSILON)
		{
			auto decl = decls_->child;
			auto vtype = decl->child;
			SYMBOL_TYPE type = vtype->child->type == T::INT ? SYMBOL_TYPE::INT : SYMBOL_TYPE::CHAR;
			auto words = vtype->sibling;
			assignVariables(words, type, SYMBOL_SCOPE::BLOCK_LOCAL, block_level, block_id);
			decls_ = decl->sibling;
		}

	}

	void assignVariables(Parse_Node * words, SYMBOL_TYPE type, SYMBOL_SCOPE scope, unsigned int block_level, long long & block_id)
	{
		auto word = words->child;
		if (word->type != T::EPSILON)
		{
			
			if (!info.symbol_table->insertData(word->data, block_id, new SymbolData(type, scope, block_level, (type == SYMBOL_TYPE::INT ? 4 : 1))))
			{
				*info.error = *info.error | 0x8;
			}
			auto words_ = word->sibling;

			while (words_->child->type != T::EPSILON)
			{
				word = words_->child->sibling;
				if(!info.symbol_table->insertData(word->data, block_id, new SymbolData(type, scope, block_level, (type == SYMBOL_TYPE::INT ? 4 : 1))))
				{
					*info.error = *info.error | 0x8;
				}
				words_ = word->sibling;
			}
		}
	}

	void showError()
	{
		if (*info.error & 0x1)
		{
			printf("Syntax Error: not defined variable used\n");
		}
		if (*info.error & 0x2)
		{
			printf("Syntax Error: not defined function used\n");
		}
		if (*info.error & 0x4)
		{
			printf("Syntax Error: redefinition of function\n");
		}
		if (*info.error & 0x8)
		{
			printf("Syntax Error: redefinition of varaible\n");
		}
	}

	void handleError(Parse_Node * node, long long & block_id)
	{
		
		//선언 없이 사용한 함수
		if (node->type == T::word && node->sibling && node->sibling->child && node->sibling->child->type == T::parenO)
		{
			if (!info.symbol_table->getData(node->data, 0))
				*info.error = *info.error | 0x2;
		}
		//scoping rule in-function var or my block var
		//선언 없이 사용한 변수 & 스코프에 없는 변수
		else if (node->type == T::word)
		{
			int pid = info.parent_id[block_id];
			int count = 0;

			if (info.symbol_table->getData(node->data, block_id))
			{
				count++;
			}
			if (info.symbol_table->getData(node->data, pid))
			{
				count++;
			}
			
			if (count == 0)
			{
				*info.error = *info.error | 0x1;
			}
		}
	}
}
