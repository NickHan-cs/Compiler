#include <string>
#include <iostream>
#include <utility>
#include <cstdlib>
#include "InputFile.h"
#include "Lexer.h"
using namespace std;
const bool LEXER_OUTPUT = true;
int main() {
	ifstream infile;
	ofstream outfile;
	string data;
	InputFile input_file("testfile.txt");
	if (LEXER_OUTPUT) {
		outfile.open("output.txt", ios::out);
	}
	pair<char, int> ch_pair = input_file.getch();
	while (ch_pair.second != 0) {
		while (isspace(ch_pair.first)) {
			ch_pair = input_file.getch();
		}	// 跳过空白符
		char ch = ch_pair.first;
		int line = ch_pair.second;
		string token_str = "";
		int token_sym = -1;
		if (isalpha(ch) or ch == '_') {	// 判断当前字符是否是一个字母
			while (isalpha(ch) or ch == '_' or isdigit(ch)) {	// 将字符拼接成字符串
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
			int num = atoi(token_str.c_str());	// 将token中的字符串转换成整数
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
		ch_pair = input_file.getch();
	}
	return 0;
}