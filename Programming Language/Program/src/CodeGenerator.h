#pragma once

#include"AST.h"
#include"SymbolTable.h"

#include<string>
#include<sstream>

class CodeGenerator
{
public:
	CodeGenerator(const std::string & filename);
	void generate(std::map<std::string, AST::AST_Node*> & ast, SymbolTable & symbol_table);

private:
	std::string gen(AST::AST_Node * node, int level);

private:
	std::map<std::string, int> func_map;
	std::string filename;
	int label;
	int Reg_max;
	int AReg_max;
};