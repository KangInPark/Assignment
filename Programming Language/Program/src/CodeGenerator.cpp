#include "CodeGenerator.h"
#include <iostream>
#include<fstream>
#include<sstream>
#include<algorithm>

using namespace std;

CodeGenerator::CodeGenerator(const string & filename)
{
	this->filename = filename;
}

void CodeGenerator::generate(map<string, AST::AST_Node*> & ast, SymbolTable & symbol_table)
{
	stringstream ss;
	Reg_max = -1;
	AReg_max = -1;
	if (ast.find("main") == ast.end()) {
		printf("no main function entry\n");
		return;
	}
	int cnt = 0;
	label = ast.size() - 1;
	for (map<string, AST::AST_Node*>::iterator i = ast.begin(); i != ast.end(); ++i) {
		func_map[i->first] = cnt++;
	}
	auto node = ast["main"];

	while (node)
	{
		ss << gen(node, 0);
		node = node->next;
	}	
	ss << "\tEND\n\n";

	for (map<string, AST::AST_Node*>::iterator i = ast.begin(); i != ast.end(); ++i) {
		if (i->first == "main")continue;
		auto node = ast[i->first];
		ss << "label" + to_string(func_map[i->first]) + "::";
		while (node)
		{
			ss << gen(node, 0);
			node = node->next;
		}
		ss << "\tJUMP\tReturn_Adress\n\n";
	}
	ofstream out(filename);

	string output = ss.str();
	output += string("\nNumber of using Registers : ") + to_string(Reg_max + AReg_max + 2) + "\n[Reg: " + to_string(Reg_max + 1) + " // AReg: " + to_string(AReg_max + 1) + "]";
	//cout << output;
	out << output;
	out.close();
}

string CodeGenerator::gen(AST::AST_Node * node, int level)
{
	string code = "";
	node->level = level;
	if (Reg_max < level)Reg_max = level;
	if (node->type == T::RETURN) return code;
	
	if (node->type == T::$) {
		int alevel = 0;
		auto nodet = node;
		while (nodet->left) {
			nodet = nodet->left;
			code += string("\tLD\tA_Reg#") + to_string(alevel++) + ", " + nodet->data + "\n";
		}
		if (AReg_max < alevel-1)AReg_max = alevel-1;
		code += string("\tCALL\t") + node->data + ", label" + to_string(func_map[node->data]) + "\n";
		return code;
	}
	if (node->is_leaf) {
		code += "\tLD\tReg#";
		code += to_string(node->level);
		code += ", ";
		code += node->data;
		code += "\n";
		return code;
	}
	else if (node->type == T::IF) {
		code += gen(node->left, level);
		code += gen(node->F, level);
		code += "\tJUMP\tlabel";
		code += to_string(label + 1);
		code += "\nlabel";
		code += to_string(label);
		code += "::";
		code += gen(node->T, level);
		code += "label";
		code += to_string(label + 1);
		code += "::";
		label += 2;
	}
	else if (node->type == T::WHILE) {
		code += "\tJUMP\tlabel";
		code += to_string(label + 1) + "\n";
		code += "label";
		code += to_string(label);
		code += "::";
		code += gen(node->T, level);
		code += "label";
		code += to_string(label+1);
		code += "::";
		code += gen(node->left, level);
		label += 2;
	}
	else {
		if (node->left && node->type!=T::ASSIGN) {
			code += gen(node->left, level);
		}
		if (node->right) {
			int levelt = node->type == T::ASSIGN ? level : level + 1;
			code += gen(node->right, levelt);
		}
	}
	switch (node->type)
	{
	case T::ADD:
		code += "\tADD\tReg#";
		code += to_string(node->level);
		code += ", Reg#";
		code += to_string(node->left->level);
		code += ", Reg#";
		code += to_string(node->right->level);
		break;
	case T::MUL:
		code += "\tMUL\tReg#";
		code += to_string(node->level);
		code += ", Reg#";
		code += to_string(node->left->level);
		code += ", Reg#";
		code += to_string(node->right->level);
		break;
	case T::ASSIGN:
		code += "\tST\tReg#";
		code += to_string(node->right->level);
		code += ", ";
		code += node->left->data;
		break;
	case T::EQUAL:
		code += "\tLT\tReg#"; code += to_string(node->level); code += ", Reg#";	code += to_string(node->left->level); code += ", REG#"; code += to_string(node->right->level);
		code += '\n';
		code += "\tLT\tReg#"; code += to_string(node->right->level + 1); code += ", Reg#"; code += to_string(node->right->level); code += ", REG#"; code += to_string(node->left->level);
		code += "\n";
		code += "\tADD\tReg#"; code += to_string(node->level); code += ", Reg#"; code += to_string(node->level); code += ", Reg#"; code += to_string(node->right->level + 1);
		code += "\n";
		code += "\tJUMPF\tReg#"; code += to_string(node->level); code += "  label"; code += to_string(label);
		if (Reg_max < node->right->level + 1)Reg_max = node->right->level + 1;
		break;
	case T::INEQUAL:
		code += "\tLT\tReg#"; code += to_string(node->level); code += ", Reg#"; code += to_string(node->right->level); code += ", REG#"; code += to_string(node->left->level);
		code += '\n';
		code += "\tJUMPT\tReg#"; code += to_string(node->level); code += "  label"; code += to_string(label);
		break;
	default:
		return code;
	}
	code += "\n";
	return code;
}
