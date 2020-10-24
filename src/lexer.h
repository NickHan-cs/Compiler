#pragma once
#include <stack>
#include <iostream>
#include <cctype>
#include <string>
#include <vector>
#include <utility>
#include <cstdlib>
#include <algorithm>
#include <unordered_map>
#include "InputFile.h"
using namespace std;
// 类别码编号
constexpr auto IDENFR = 0;		// 标识符
constexpr auto INTCON = 1;		// 整型常量
constexpr auto CHARCON = 2;		// 字符常量
constexpr auto STRCON = 3;		// 字符串
constexpr auto CONSTTK = 4;		// const
constexpr auto INTTK = 5;		// int
constexpr auto CHARTK = 6;		// char
constexpr auto VOIDTK = 7;		// void
constexpr auto MAINTK = 8;		// main
constexpr auto IFTK = 9;		// if
constexpr auto ELSETK = 10;		// else
constexpr auto SWITCHTK = 11;	// switch
constexpr auto CASETK = 12;		// case
constexpr auto DEFAULTTK = 13;	// default
constexpr auto WHILETK = 14;	// while
constexpr auto FORTK = 15;		// for
constexpr auto SCANFTK = 16;	// scanf
constexpr auto PRINTFTK = 17;	// printf
constexpr auto RETURNTK = 18;	// return
constexpr auto PLUS = 19;		// +
constexpr auto MINU = 20;		// -
constexpr auto MULT = 21;		// *
constexpr auto DIV = 22;		// /
constexpr auto LSS = 23;		// <
constexpr auto LEQ = 24;		// <=
constexpr auto GRE = 25;		// >
constexpr auto GEQ = 26;		// >=
constexpr auto EQL = 27;		// ==
constexpr auto NEQ = 28;		// !=
constexpr auto COLON = 29;		// :
constexpr auto ASSIGN = 30;		// =
constexpr auto SEMICN = 31;		// ;
constexpr auto COMMA = 32;		// ,
constexpr auto LPARENT = 33;	// (
constexpr auto RPARENT = 34;	// )
constexpr auto LBRACK = 35;		// [
constexpr auto RBRACK = 36;		// ]
constexpr auto LBRACE = 37;		// {
constexpr auto RBRACE = 38;		// }

// 类别码表
vector<string> symbol_table = {
	"IDENFR",
	"INTCON",
	"CHARCON",
	"STRCON",
	"CONSTTK",
	"INTTK",
	"CHARTK",
	"VOIDTK",
	"MAINTK",
	"IFTK",
	"ELSETK",
	"SWITCHTK",
	"CASETK",
	"DEFAULTTK",
	"WHILETK",
	"FORTK",
	"SCANFTK",
	"PRINTFTK",
	"RETURNTK",
	"PLUS",
	"MINU",
	"MULT",
	"DIV",
	"LSS",
	"LEQ",
	"GRE",
	"GEQ",
	"EQL",
	"NEQ",
	"COLON",
	"ASSIGN",
	"SEMICN",
	"COMMA",
	"LPARENT",
	"RPARENT",
	"LBRACK",
	"RBRACK",
	"LBRACE",
	"RBRACE"
};

unordered_map<string, int> reserver_map{
	{"const",	CONSTTK},
	{"int",		INTTK},
	{"char",	CHARTK},
	{"void",	VOIDTK},
	{"main",	MAINTK},
	{"if",		IFTK},
	{"else",	ELSETK},
	{"switch",	SWITCHTK},
	{"case",	CASETK},
	{"default", DEFAULTTK},
	{"while",	WHILETK},
	{"for",		FORTK},
	{"scanf",	SCANFTK},
	{"printf",	PRINTFTK},
	{"return",	RETURNTK}
};

class Token {
private:
	int line;
	int token_sym;
	string token_str;
public:
	Token(int, int, string);
	~Token(void);
	int get_line(void) { return line; }
	int get_token_sym(void) { return token_sym; }
	string get_token_str(void) { return token_str; }
};

Token::Token(int line, int token_sym, string token_str) {
	this->line = line;
	this->token_sym = token_sym;
	this->token_str = token_str;
}

Token::~Token(void) {
}

extern InputFile input_file;
extern const bool LEXER_OUTPUT;
extern stack<Token> token_stack;
extern ofstream outfile;

namespace Lexer {
	int getSymbol(string token_str) {
		transform(token_str.begin(), token_str.end(), token_str.begin(), ::tolower);
		if (reserver_map.find(token_str) != reserver_map.end()) {
			return reserver_map.at(token_str);
		}
		return 0;
	}

	bool isChar(char ch) {
		return isalpha(ch) || isdigit(ch) || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '_';
	}

	bool isCharOfString(char ch) {
		int asc = ch;
		return asc >= 32 && asc <= 126 && asc != 34;
	}

	Token get_new_sym() {
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
			input_file.retract();	// 指针后退一个字符
			token_sym = Lexer::getSymbol(token_str);	// 判断是否保留字，否则为标识符
		}
		else if (isdigit(ch)) {	//	判断当前字符是否是一个数字
			while (isdigit(ch)) {	// 将字符拼接成整数
				token_str += ch;
				ch = input_file.getch().first;
			}
			input_file.retract();
			// int num = atoi(token_str.c_str());	// 将token中的字符串转换成整数
			token_sym = INTCON;	// 此时识别的单词是整数常量
		}
		else if (ch == '\'') {	// 判断当前字符是否是单引号
			ch = input_file.getch().first;
			if (Lexer::isChar(ch)) {
				token_str += ch;
				ch = input_file.getch().first;
				if (ch == '\'') {
					token_sym = CHARCON;
				}
				else {
					input_file.retract();
				}
			}
			else {
				input_file.retract();
			}
		}
		else if (ch == '\"') {
			ch = input_file.getch().first;
			if (Lexer::isCharOfString(ch)) {
				while (Lexer::isCharOfString(ch)) {
					token_str += ch;
					ch = input_file.getch().first;
				}
				if (ch == '\"') {
					token_sym = STRCON;
				}
				else {
					input_file.retract();
				}
			}
			else {
				input_file.retract();
			}
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
				input_file.retract();
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
				input_file.retract();
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
				input_file.retract();
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
				input_file.retract();
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
		if (token_sym >= 0 && LEXER_OUTPUT) {
			outfile << symbol_table[token_sym] << " " << token_str << endl;
		}
		Token token(line, token_sym, token_str);
		return token;
	}

	Token getsym() {
		if (!token_stack.empty()) {
			Token token = token_stack.top();
			token_stack.pop();
			return token;
		}
		return Lexer::get_new_sym();
	}
}