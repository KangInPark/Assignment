#include "Lexer.h"

Lexer::Lexer(const std::string & filename)
{
	src_file.open(filename);
	makeDFATable();
}

Token Lexer::getNextToken()
{
	Token token;
	token.data = "";
	char letter;

	bool start = false;

	int state = STATE::START;
	token.type = END;

	if (src_file.eof() || !src_file.is_open()) {
		if (src_file.is_open())
			src_file.close();
	}
	while (!src_file.eof())
	{
		letter = buffer == ' ' ? src_file.get() : buffer;
		if (letter == -1) break;
		buffer = ' ';

		if (!start && ((letter >= 0 && letter <= 32) || (letter == ' ' || letter == '\t' || letter == '\n') ) )
			continue;

		int input;
		if ((input = getInput(letter)) == INPUT::ERROR) {
			printf("Lexer::getNextToken(), %c, %d: letter not defined\n", letter, letter);
			token.type = ERROR;
			return token;
		}
		else
		{
			start = true;
			token.type = getTokenType(state);

			if (input == INPUT::SPACE)
			{
				break;
			}
			if (dfa_table[state][input] == STATE::MISS)
			{
				//lexical error not defined token
				if (!dfa_end[state])
				{
					token.type = TOKEN_TYPE::ERROR;
					break;
				}
				buffer = letter;
				break;
			}

			state = dfa_table[state][input];
			token.data += letter;
		}
	}

	return token;
}


void Lexer::makeDFATable()
{

	//rows state
	//cols inputs
	for (int i = 0; i < STATE::SIZE; i++)
	{
		for (int j = 0; j < INPUT::SIZE; j++)
			dfa_table[i][j] = STATE::MISS;
		dfa_end[i] = false;
	}

	//END STATE CHECK======================================================
	for (int i = STATE::LETTER; i < STATE::LETTER_END; i++)
	{
		dfa_end[i] = true;
	}
	dfa_end[STATE::DIGIT]
		= dfa_end[STATE::PLUS]			= dfa_end[STATE::MUL]		= dfa_end[STATE::ASSIGN]
		= dfa_end[STATE::EQUAL]			= dfa_end[STATE::LARGER]	= dfa_end[STATE::SBLOCK]
		= dfa_end[STATE::EBLOCK]		= dfa_end[STATE::SPAREN]	= dfa_end[STATE::EPAREN]
		= dfa_end[STATE::COMMA]			= dfa_end[STATE::SEMICOLON] = true;

	//STATE START==========================================================
	for (int i = INPUT::A; i < INPUT::z; i++) {
		dfa_table[STATE::START][i] = STATE::LETTER;
	}
	for (int i = 0; i < 10; i++) {
		dfa_table[STATE::START][i] = STATE::DIGIT;
	}
	dfa_table[STATE::START][INPUT::I]			= STATE::IF_START;
	dfa_table[STATE::START][INPUT::T]			= STATE::THEN_START;
	dfa_table[STATE::START][INPUT::E]			= STATE::ELSE_START;
	dfa_table[STATE::START][INPUT::W]			= STATE::WHILE_START;
	dfa_table[STATE::START][INPUT::R]			= STATE::RETURN_START;
	dfa_table[STATE::START][INPUT::i]			= STATE::INT_START;
	dfa_table[STATE::START][INPUT::c]			= STATE::CHAR_START;
	dfa_table[STATE::START][INPUT::PLUS]		= STATE::PLUS;
	dfa_table[STATE::START][INPUT::MUL]			= STATE::MUL;
	dfa_table[STATE::START][INPUT::ASSIGN]		= STATE::ASSIGN;
	dfa_table[STATE::START][INPUT::LARGER]		= STATE::LARGER;
	dfa_table[STATE::START][INPUT::SBLOCK]		= STATE::SBLOCK;
	dfa_table[STATE::START][INPUT::EBLOCK]		= STATE::EBLOCK;
	dfa_table[STATE::START][INPUT::SPAREN]		= STATE::SPAREN;
	dfa_table[STATE::START][INPUT::EPAREN]		= STATE::EPAREN;
	dfa_table[STATE::START][INPUT::SEMICOLON]	= STATE::SEMICOLON;
	dfa_table[STATE::START][INPUT::COMMA]		= STATE::COMMA;

	dfa_table[STATE::ASSIGN][INPUT::ASSIGN]		= STATE::EQUAL;

	//STATE DIGIT===========================================================
	for (int i = 0; i < 10; i++) {
		dfa_table[STATE::DIGIT][i] = STATE::DIGIT;
	}

	//STATE LETTER==========================================================
	for (int i = INPUT::A; i < INPUT::z; i++) {
		dfa_table[STATE::LETTER][i] = STATE::LETTER;
	}

	//STATE IF==============================================================
	for (int i = INPUT::A; i < INPUT::z; i++) {
		dfa_table[STATE::IF_START][i] = STATE::LETTER;
		dfa_table[STATE::IF_END][i] = STATE::LETTER;
	}
	dfa_table[STATE::IF_START][INPUT::F] = STATE::IF_END;

	//STATE THEN============================================================
	for (int i = INPUT::A; i < INPUT::z; i++) {
		dfa_table[STATE::THEN_START][i] = STATE::LETTER;
		dfa_table[STATE::THEN_START + 1][i] = STATE::LETTER;
		dfa_table[STATE::THEN_START + 2][i] = STATE::LETTER;
		dfa_table[STATE::THEN_END][i] = STATE::LETTER;
	}
	dfa_table[STATE::THEN_START][INPUT::H] = STATE::THEN_START + 1;
	dfa_table[STATE::THEN_START + 1][INPUT::E] = STATE::THEN_START + 2;
	dfa_table[STATE::THEN_START + 2][INPUT::N] = STATE::THEN_END;

	//STATE ELSE============================================================
	for (int i = INPUT::A; i < INPUT::z; i++) {
		dfa_table[STATE::ELSE_START][i] = STATE::LETTER;
		dfa_table[STATE::ELSE_START + 1][i] = STATE::LETTER;
		dfa_table[STATE::ELSE_START + 2][i] = STATE::LETTER;
		dfa_table[STATE::ELSE_END][i] = STATE::LETTER;
	}
	dfa_table[STATE::ELSE_START][INPUT::L] = STATE::ELSE_START + 1;
	dfa_table[STATE::ELSE_START + 1][INPUT::S] = STATE::ELSE_START + 2;
	dfa_table[STATE::ELSE_START + 2][INPUT::E] = STATE::ELSE_END;

	//STATE WHILE===========================================================
	for (int i = INPUT::A; i < INPUT::z; i++) {
		dfa_table[STATE::WHILE_START][i] = STATE::LETTER;
		dfa_table[STATE::WHILE_START + 1][i] = STATE::LETTER;
		dfa_table[STATE::WHILE_START + 2][i] = STATE::LETTER;
		dfa_table[STATE::WHILE_START + 3][i] = STATE::LETTER;
		dfa_table[STATE::WHILE_END][i] = STATE::LETTER;
	}
	dfa_table[STATE::WHILE_START][INPUT::H] = STATE::WHILE_START + 1;
	dfa_table[STATE::WHILE_START + 1][INPUT::I] = STATE::WHILE_START + 2;
	dfa_table[STATE::WHILE_START + 2][INPUT::L] = STATE::WHILE_START + 3;
	dfa_table[STATE::WHILE_START + 3][INPUT::E] = STATE::WHILE_END;

	//STATE RETURN===========================================================
	for (int i = INPUT::A; i < INPUT::z; i++) {
		dfa_table[STATE::RETURN_START][i] = STATE::LETTER;
		dfa_table[STATE::RETURN_START + 1][i] = STATE::LETTER;
		dfa_table[STATE::RETURN_START + 2][i] = STATE::LETTER;
		dfa_table[STATE::RETURN_START + 3][i] = STATE::LETTER;
		dfa_table[STATE::RETURN_START + 4][i] = STATE::LETTER;
		dfa_table[STATE::RETURN_END][i] = STATE::LETTER;
	}
	dfa_table[STATE::RETURN_START][INPUT::E] = STATE::RETURN_START + 1;
	dfa_table[STATE::RETURN_START + 1][INPUT::T] = STATE::RETURN_START + 2;
	dfa_table[STATE::RETURN_START + 2][INPUT::U] = STATE::RETURN_START + 3;
	dfa_table[STATE::RETURN_START + 3][INPUT::R] = STATE::RETURN_START + 4;
	dfa_table[STATE::RETURN_END][INPUT::N] = STATE::RETURN_END;

	//STATE int============================================================
	for (int i = INPUT::A; i < INPUT::z; i++) {
		dfa_table[STATE::INT_START][i] = STATE::LETTER;
		dfa_table[STATE::INT_START + 1][i] = STATE::LETTER;
		dfa_table[STATE::INT_END][i] = STATE::LETTER;
	}
	dfa_table[STATE::INT_START][INPUT::n] = STATE::INT_START + 1;
	dfa_table[STATE::INT_START + 1][INPUT::t] = STATE::INT_END;

	//STATE char============================================================
	for (int i = INPUT::A; i < INPUT::z; i++) {
		dfa_table[STATE::CHAR_START][i] = STATE::LETTER;
		dfa_table[STATE::CHAR_START + 1][i] = STATE::LETTER;
		dfa_table[STATE::CHAR_START + 2][i] = STATE::LETTER;
		dfa_table[STATE::CHAR_END][i] = STATE::LETTER;
	}
	dfa_table[STATE::CHAR_START][INPUT::h] = STATE::CHAR_START + 1;
	dfa_table[STATE::CHAR_START + 1][INPUT::a] = STATE::CHAR_START + 2;
	dfa_table[STATE::CHAR_START + 2][INPUT::r] = STATE::CHAR_END;

}

int Lexer::getInput(const char c)
{
	if (isdigit(c))
		return c - '0';
	else if (isalpha(c))
	{
		if (isupper(c))
			return c - 'A' + INPUT::A;
		else
			return c - 'a' + INPUT::a;
	}
	else {
		switch (c)
		{
		case '+': return INPUT::PLUS;
		case '*': return INPUT::MUL;
		case '=': return INPUT::ASSIGN;
		case '>': return INPUT::LARGER;
		case '{': return INPUT::SBLOCK;
		case '}': return INPUT::EBLOCK;
		case '(': return INPUT::SPAREN;
		case ')': return INPUT::SPAREN;
		case ';': return INPUT::SEMICOLON;
		case ',': return INPUT::COMMA;
		case ' ': case '\t': case '\n': return INPUT::SPACE;
		default: 
			if (c >= 0 && c <= 32) {
				return INPUT::SPACE;
			}
			return INPUT::ERROR;
		}
	}
}

TOKEN_TYPE Lexer::getTokenType(int state)
{
	TOKEN_TYPE type = TOKEN_TYPE::ERROR;
	for (int i = STATE::LETTER; i < STATE::LETTER_END; i++)
	{
		if (i == state) type = TOKEN_TYPE::IDENTIFIER;
	}
	switch (state)
	{
	case STATE::IF_END :
	case STATE::THEN_END:
	case STATE::ELSE_END:
	case STATE::WHILE_END:
	case STATE::RETURN_END:
	case STATE::INT_END:
	case STATE::CHAR_END:
		type = TOKEN_TYPE::KEYWORD;
		break;
		
	case STATE::PLUS: case STATE::ASSIGN: case STATE::MUL:
	case STATE::EQUAL: case STATE::LARGER:
		type = TOKEN_TYPE::OPERATOR;
		break;

	case STATE::SBLOCK: case STATE::EBLOCK:
	case STATE::SPAREN: case STATE::EPAREN:
	case STATE::SEMICOLON: case STATE::COMMA:
		type = TOKEN_TYPE::SEPERATOR;
		break;
	case STATE::DIGIT:
		type = TOKEN_TYPE::LITERAL;
		break;

	}
	return type;
}
