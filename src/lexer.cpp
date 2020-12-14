#include <fstream>
#include <string>
#include <algorithm>
#include <utility>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <utility>
#include <cstdlib>
#include "lexer.h"
#include "error.h"

using namespace std;

extern InputFile input_file;
extern ofstream output_file;

Token::Token(int line, int token_sym, string token_str) {
	this->line = line;
	this->token_sym = token_sym;
	this->token_str = token_str;
}

Token::~Token(void) {
}

int lexer::GetReserverNum(string token_str) {
	// token_str大写转小写
	transform(token_str.begin(), token_str.end(), token_str.begin(), ::tolower);
	if (reserver_map.find(token_str) != reserver_map.end()) {
		return reserver_map.at(token_str);
	}
	return 0;
}

bool lexer::IsChar(char ch) {
	return isalpha(ch) || isdigit(ch) || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '_';
}

bool lexer::IsCharOfString(char ch) {
	int asc = ch;
	return asc >= 32 && asc <= 126 && asc != 34;
}

Token lexer::GetNewToken() {
	pair<char, int> ch_pair = input_file.getch();
	if (ch_pair.second == 0) {
		Token token(0, -1, "");
		return token;
	}
	while (isspace(ch_pair.first)) {
		ch_pair = input_file.getch();
	}	// 跳过空白符
	char ch = ch_pair.first;
	int line = ch_pair.second;
	string token_str = "";
	int token_sym = -1;
	if (isalpha(ch) || ch == '_') {	// 判断当前字符是否是一个字母
		while (isalpha(ch) || ch == '_' || isdigit(ch)) {	// 将字符拼接成字符串
			token_str += ch;
			ch = input_file.getch().first;
		}
		input_file.Retract();	// 指针后退一个字符
		token_sym = lexer::GetReserverNum(token_str);	// 判断是否保留字，否则为标识符
	}
	else if (isdigit(ch)) {	//	判断当前字符是否是一个数字
		while (isdigit(ch)) {	// 将字符拼接成整数
			token_str += ch;
			ch = input_file.getch().first;
		}
		input_file.Retract();
		token_sym = INTCON;	// 此时识别的单词是整数常量
	}
	else if (ch == '\'') {	// 判断当前字符是否是单引号
		ch = input_file.getch().first;
		if (lexer::IsChar(ch)) {
			token_str += ch;
			ch = input_file.getch().first;
			if (ch != '\'') {
				// 非法符号或不符合词法
				error::OutputError(line, InvalidSymbol);
				while (ch != '\'') {
					ch = input_file.getch().first;
				}
			}
		}
		else {
			// 非法符号或不符合词法
			error::OutputError(line, InvalidSymbol);
			// 如果没有'会死循环
			while (ch != '\'') {
				ch = input_file.getch().first;
			}
		}
		token_sym = CHARCON;
	}
	else if (ch == '\"') {
		ch = input_file.getch().first;
		if (ch == '\"') {
			// 符号串中无任何符号
			// 因为不会出现恶意换行的情况，所以错误所在行就是前双引号所在行
			error::OutputError(line, InvalidSymbol);
		}
		else {
			while (ch != '\"') {
				// 如果没有后双引号，就会死循环
				if (lexer::IsCharOfString(ch)) {
					// 非法字符不加入字符串中
					token_str += ch;
					if (ch == '\\') {
						token_str += ch;
					}
				}
				else {
					// 因为不会出现恶意换行的情况，所以字符串一定在同一行，错误所在行即前双引号所在行
					error::OutputError(line, InvalidSymbol);
				}
				ch = input_file.getch().first;
			}
		}
		token_sym = STRCON;
	}
	else if (ch == '+') {
		token_str += ch;
		token_sym = PLUS;
	}
	else if (ch == '-') {
		token_str += ch;
		token_sym = MINU;
	}
	else if (ch == '*') {
		token_str += ch;
		token_sym = MULT;
	}
	else if (ch == '/') {
		token_str += ch;
		token_sym = DIV;
	}
	else if (ch == '<') {
		token_str += ch;
		ch = input_file.getch().first;
		if (ch == '=') {
			token_str += ch;
			token_sym = LEQ;
		}
		else {
			input_file.Retract();
			token_sym = LSS;
		}
	}
	else if (ch == '>') {
		token_str += ch;
		ch = input_file.getch().first;
		if (ch == '=') {
			token_str += ch;
			token_sym = GEQ;
		}
		else {
			input_file.Retract();
			token_sym = GRE;
		}
	}
	else if (ch == '=') {
		token_str += ch;
		ch = input_file.getch().first;
		if (ch == '=') {
			token_str += ch;
			token_sym = EQL;
		}
		else {
			input_file.Retract();
			token_sym = ASSIGN;
		}
	}
	else if (ch == '!') {
		token_str += ch;
		ch = input_file.getch().first;
		if (ch == '=') {
			token_str += ch;
			token_sym = NEQ;
		}
		else {
			input_file.Retract();
		}
	}
	else if (ch == ':') {
		token_str += ch;
		token_sym = COLON;
	}
	else if (ch == ';') {
		token_str += ch;
		token_sym = SEMICN;
	}
	else if (ch == ',') {
		token_str += ch;
		token_sym = COMMA;
	}
	else if (ch == '(') {
		token_str += ch;
		token_sym = LPARENT;
	}
	else if (ch == ')') {
		token_str += ch;
		token_sym = RPARENT;
	}
	else if (ch == '[') {
		token_str += ch;
		token_sym = LBRACK;
	}
	else if (ch == ']') {
		token_str += ch;
		token_sym = RBRACK;
	}
	else if (ch == '{') {
		token_str += ch;
		token_sym = LBRACE;
	}
	else if (ch == '}') {
		token_str += ch;
		token_sym = RBRACE;
	}
	else {
		// 根据题目要求，似乎不会出现这种情况
	}
	if (token_sym >= 0 && LEXER_OUTPUT) {
		output_file << token_table[token_sym] << " " << token_str << endl;
	}
	Token token(line, token_sym, token_str);
	return token;
}