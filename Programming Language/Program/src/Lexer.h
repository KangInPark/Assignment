#pragma once

#include<string>
#include<fstream>
#include"SymbolTable.h"

enum TOKEN_TYPE {
	ERROR='e', IDENTIFIER='i', KEYWORD='k', SEPERATOR='s', OPERATOR='o', LITERAL='l', END='d'
};

struct Token
{
	std::string data;
	TOKEN_TYPE type;
};

namespace INPUT {
	enum  {
		SPACE = -1,
		//0~9
		A = 10, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
		PLUS, ASSIGN, MUL, LARGER, SBLOCK, EBLOCK, SPAREN, EPAREN, SEMICOLON, COMMA,
		ERROR,
		SIZE
	};
}

namespace STATE {
	enum  {
		START = 0,
		MISS = -1,
		LETTER = 1,
		IF_START = 2, IF_END = 3,
		THEN_START = 4, THEN_END = 7,
		ELSE_START = 8, ELSE_END = 11,
		WHILE_START = 12, WHILE_END = 16,
		RETURN_START = 17, RETURN_END = 21,
		INT_START = 22, INT_END = 24,
		CHAR_START = 25, CHAR_END = 28, LETTER_END = 29,
		PLUS = 29, ASSIGN = 30, MUL = 31,
		EQUAL = 32,
		LARGER = 33,
		SBLOCK = 34, EBLOCK = 35,
		SPAREN = 36, EPAREN = 37,
		SEMICOLON = 38,
		COMMA = 39,
		DIGIT = 40,
		SIZE
	};
}

class Lexer
{

public:
	Lexer(const std::string &filename);

	Token getNextToken();

private:
	void makeDFATable();
	int getInput(const char c);
	TOKEN_TYPE getTokenType(int state);

private:
	std::ifstream src_file;

	int dfa_table[STATE::SIZE][INPUT::SIZE];
	bool dfa_end[STATE::SIZE];
	char buffer = ' ';
};