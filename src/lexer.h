#pragma once
#include <vector>
#include <unordered_map>
# include "input_file.h"

const bool LEXER_OUTPUT = false;

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
const std::vector<std::string> token_table = {
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

// 保留字表
const std::unordered_map<std::string, int> reserver_map{
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
	std::string token_str;
public:
	Token(int, int, std::string);
	~Token(void);
	int get_line(void) { return line; }
	int get_token_sym(void) { return token_sym; }
	std::string get_token_str(void) { return token_str; }
};

namespace lexer {
	// 如果token_str是保留字，GetReserverNum返回其类别码编号；否则，返回0
	int GetReserverNum(std::string token_str);
	bool IsChar(char ch);
	bool IsCharOfString(char ch);
	Token GetNewToken();
}