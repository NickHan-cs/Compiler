#pragma once
#include <map>
#include <string>
#include <fstream>
#include <utility>

const bool LEXER_INPUT_DEBUG = false;

class InputFile {
private:
	std::string file_path;
	int cur_col = 0;  // start from 1
	int cur_line = 0; // start from 1
	int cur_line_len = -1; 
	std::string cur_line_code;
	std::map<int, int> len_lines;
public:
	std::ifstream infile;
	InputFile(std::string);
	~InputFile();
	std::string get_file_path(void);
	// getch返回文件中下一个字符，如果读到文件末，返回 null_pair(0, 0)
	std::pair<char, int> getch(void);
	// get_line_len返回第line行的长度
	int get_line_len(int line);
	// 在读取文件时，回退一个字符
	void Retract(void);
};