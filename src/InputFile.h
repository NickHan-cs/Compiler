#pragma once
#include <string>
#include <fstream>
#include <utility>
#include <map>
using namespace std;

const bool LEXER_INPUT_DEBUG = false;

class InputFile {
private:
	string file_path;
	int cur_col = 0;  // start from 1
	int cur_line = 0; // start from 1
	int cur_line_len = -1; 
	string cur_line_code;
	map<int, int> len_lines;
public:
	ifstream infile;
	InputFile(std::string);
	~InputFile();
	string getFilePath(void);
	pair<char, int> getch(void);
	int getLineLen(int line);
	void retract(void);
};