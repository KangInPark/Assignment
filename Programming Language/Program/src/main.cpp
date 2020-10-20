#include <string>
#include <iostream>
#include "Lexer.h"
#include "Parser.h"
#include "SymbolTable.h"
#include "CodeGenerator.h"
#include "AST.h"

#ifdef _WIN32
#include<direct.h>
#else
#include<sys/stat.h>
#endif

std::string getFileName(const std::string& s) 
{
	char sep = '/';

#ifdef _WIN32
	sep = '\\';
#endif

	size_t i = s.rfind(sep, s.length());
	if (i != std::string::npos) {
		return(s.substr(i + 1, s.length() - i));
	}

	return("");
}


int main(int argc, char **argv)
{
	if (argc == 1) {
		printf("Need input file's Name.");
		return 0;
	}
#ifdef _WIN32
	_mkdir("result");
#else
	mkdir("result", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
	std::string name = getFileName(argv[1]);
	std::string inpath = argv[1];
	std::string outpath = std::string("result/") + name + std::string(".code");

	SymbolTable symbol_table = SymbolTable();
	Lexer lex = Lexer(inpath);
	Parser parser = Parser();
	if (parser.Parsing(lex))return -1;
	int error;
	auto root = AST::getAST(parser.get_Root(), &symbol_table, &error);
	if (error)
		return -1;
	CodeGenerator codegenerator = CodeGenerator(outpath);
	codegenerator.generate(root, symbol_table);
	symbol_table.print("result/" + name + ".symbol");
	printf("Compile Finished!\n");
	return 0;
}