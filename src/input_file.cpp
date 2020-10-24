#include <iostream>
#include <cassert>
#include "input_file.h"

using namespace std;

InputFile::InputFile(string file_path) {
	this->file_path = file_path;
	infile.open(file_path, ios::in);
	cur_col = 0;
	cur_line = 0;
	cur_line_len = -1;
	cur_line_code = "";
	if (LEXER_INPUT_DEBUG) {
		cout << "Starting Lexical Analyse of " << file_path << " ..." << endl;
	}
}

InputFile::~InputFile(void) {
	infile.close();
	if (LEXER_INPUT_DEBUG) {
		cout << "Close" << file_path << endl;
	}
}

pair<char, int> InputFile::getch(void) {
	if (cur_col > cur_line_len) {
		// 读到了当前行的末尾
		len_lines.emplace(cur_line, cur_line_len);
		// getline读取文件的下一行
		if (!getline(infile, cur_line_code)) {
			// 读到了文件的末尾
			if (LEXER_INPUT_DEBUG) {
				cout << "Input End..." << endl;
			}
			pair<char, int> null_pair(0, 0);
			return null_pair;
		}
		cur_col = 1;
		cur_line += 1;
		cur_line_len = cur_line_code.length();
		if (LEXER_INPUT_DEBUG) {
			// 输出这一行的内容
			cout << cur_line << " " << cur_line_len << '\t' << cur_line_code << endl;
		}
		if (cur_line_len == 0) {
			// 如果该行为空行，就再读取下一个字符
			return InputFile::getch();
		}
	}
	pair<char, int> ch(cur_line_code[cur_col-1], cur_line);
	cur_col += 1;
	return ch;
}

string InputFile::get_file_path(void) {
	return file_path;
}


int InputFile::get_line_len(int line) {
	return len_lines.at(line);
}

void InputFile::Retract(void) {
	if (LEXER_INPUT_DEBUG) {
		assert(cur_col > 1);
	}
	cur_col -= 1;
}