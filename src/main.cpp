# include "input_file.h"
# include "lexer.h"
# include "grammar.h"

using namespace std;

InputFile input_file("testfile.txt");
ofstream output_file;
Token token(0, -1, "");

int main() {
	if (LEXER_OUTPUT || GRAMMAR_OUTPUT) {
		output_file.open("output.txt", ios::out);
	}
	token = lexer::GetNewToken();
	grammar::program();
}