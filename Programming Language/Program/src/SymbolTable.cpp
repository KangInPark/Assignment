#include "SymbolTable.h"

#include<fstream>

SymbolTable::~SymbolTable()
{
	release();
}

SymbolData * SymbolTable::getData(const std::string & symbol_name, long long block_id)
{
	if (symbol_map.find(symbol_name.data()) != symbol_map.end())
	{
		auto m = symbol_map[symbol_name.data()];
		if (m.find(block_id) != m.end())
			return m[block_id];
	}
	return nullptr;
}

bool SymbolTable::insertData(const std::string & symbol_name, long long block_id, SymbolData * data)
{
	if (symbol_map.find( symbol_name.data() ) == symbol_map.end())
	{
		symbol_map.insert({ symbol_name.data(), std::map<long long, SymbolData *>() });
	}
	auto bmap = &symbol_map[symbol_name.data()];
	if (bmap->find(block_id) == bmap->end())
	{
		bmap->insert({ block_id, data });
		return true;
	}
	return false;
}

void SymbolTable::print(const std::string & filename)
{
	std::string sym_type[SYMBOL_TYPE::TYPE_SIZE];
	sym_type[SYMBOL_TYPE::CHAR] = "char";
	sym_type[SYMBOL_TYPE::INT] = "int";
	sym_type[SYMBOL_TYPE::FUNCTION_CHAR] = "F/char";
	sym_type[SYMBOL_TYPE::FUNCTION_INT] = "F/int";

	std::string sym_scope[SYMBOL_SCOPE::SCOPE_SIZE];
	sym_scope[SYMBOL_SCOPE::BLOCK_LOCAL] = "local";
	sym_scope[SYMBOL_SCOPE::GLOBAL] = "global";
	sym_scope[SYMBOL_SCOPE::FUNTION_PARAMETER] = "parameter";

	std::ofstream out(filename);

	out << "** F/char : function char, F/int : function int \n";
	out << "name" << "\t|\t" << "block id" << "\t|\t" << "type" << "\t|\t" << "scope" << "\t\t|\t" <<
		"block level" << "\t|\t" << "data size" << "\t|\t" << "address\n";
	out << "--------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
	for (auto i : symbol_map)
	{
		for (auto j : i.second)
		{
			auto data = j.second;
			out << i.first.data() << "\t|\t" <<
				j.first << "\t|\t" <<
				sym_type[data->type].data() << "\t|\t" <<
				sym_scope[data->scope].data() << "\t\t|\t" <<
				data->block_level << "\t\t|\t" <<
				data->data_size << "\t|\t" <<
				data->address << "\n";
		}
	}
	out.close();
}

void SymbolTable::release()
{
	for (auto i : symbol_map)
	{
		for (auto j : i.second)
		{
			delete j.second;
		}
	}
}
