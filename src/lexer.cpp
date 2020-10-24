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
	// token_str��дתСд
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
	}	// �����հ׷�
	char ch = ch_pair.first;
	int line = ch_pair.second;
	string token_str = "";
	int token_sym = -1;
	if (isalpha(ch) || ch == '_') {	// �жϵ�ǰ�ַ��Ƿ���һ����ĸ
		while (isalpha(ch) || ch == '_' || isdigit(ch)) {	// ���ַ�ƴ�ӳ��ַ���
			token_str += ch;
			ch = input_file.getch().first;
		}
		input_file.Retract();	// ָ�����һ���ַ�
		token_sym = lexer::GetReserverNum(token_str);	// �ж��Ƿ����֣�����Ϊ��ʶ��
	}
	else if (isdigit(ch)) {	//	�жϵ�ǰ�ַ��Ƿ���һ������
		while (isdigit(ch)) {	// ���ַ�ƴ�ӳ�����
			token_str += ch;
			ch = input_file.getch().first;
		}
		input_file.Retract();
		// int num = atoi(token_str.c_str());	// ��token�е��ַ���ת��������
		token_sym = INTCON;	// ��ʱʶ��ĵ�������������
	}
	else if (ch == '\'') {	// �жϵ�ǰ�ַ��Ƿ��ǵ�����
		ch = input_file.getch().first;
		if (lexer::IsChar(ch)) {
			token_str += ch;
			ch = input_file.getch().first;
			if (ch == '\'') {
				token_sym = CHARCON;
			}
			else {
				input_file.Retract();
			}
		}
		else {
			input_file.Retract();
		}
	}
	else if (ch == '\"') {
		ch = input_file.getch().first;
		if (lexer::IsCharOfString(ch)) {
			while (lexer::IsCharOfString(ch)) {
				token_str += ch;
				ch = input_file.getch().first;
			}
			if (ch == '\"') {
				token_sym = STRCON;
			}
			else {
				input_file.Retract();
			}
		}
		else {
			input_file.Retract();
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
	if (token_sym >= 0 && LEXER_OUTPUT) {
		output_file << symbol_table[token_sym] << " " << token_str << endl;
	}
	Token token(line, token_sym, token_str);
	return token;
}