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
	// getch�����ļ�����һ���ַ�����������ļ�ĩ������ null_pair(0, 0)
	std::pair<char, int> getch(void);
	// get_line_len���ص�line�еĳ���
	int get_line_len(int line);
	// �ڶ�ȡ�ļ�ʱ������һ���ַ�
	void Retract(void);
};