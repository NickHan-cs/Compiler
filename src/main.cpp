#include "input_file.h"
#include "lexer.h"
#include "grammar.h"
#include "error.h"

using namespace std;

InputFile input_file("testfile.txt");
ofstream output_file;
ofstream error_file;
Token last_token(0, -1, "");
Token token(0, -1, "");


int main() {
	if (LEXER_OUTPUT || GRAMMAR_OUTPUT) {
		output_file.open("output.txt", ios::out);
	}
	if (ERROR_TYPE_OUTPUT || ERROR_DESCRIPTION_OUTPUT) {
		error_file.open("error.txt", ios::out);
	}
	token = lexer::GetNewToken();
	grammar::program();
}