#pragma once

#include<unordered_map>
#include<map>

enum SYMBOL_TYPE {
	CHAR, INT, FUNCTION_CHAR, FUNCTION_INT, TYPE_SIZE
};

enum SYMBOL_SCOPE {
	FUNTION_PARAMETER, BLOCK_LOCAL, GLOBAL, SCOPE_SIZE
};

struct SymbolKey
{
	std::string name;
	unsigned int block_level;
};

struct SymbolData
{
	SymbolData(SYMBOL_TYPE stype, SYMBOL_SCOPE sscope, unsigned int blevel, unsigned int dsize)
		: type(stype), scope(sscope), block_level(blevel), data_size(dsize) {};
	SYMBOL_TYPE type;
	SYMBOL_SCOPE scope;
	unsigned int block_level;
	unsigned int data_size;
	int address;
};

class SymbolTable
{
public:
	~SymbolTable();

	SymbolData * getData(const std::string & symbol_name, long long block_id);
	bool insertData(const std::string & symbol_name, long long block_id, SymbolData * data);

	void print(const std::string & filename);

private:
	//release SymbolData in symbol_map
	void release();
private:
	std::unordered_map<std::string, std::map<long long, SymbolData *>> symbol_map;
};