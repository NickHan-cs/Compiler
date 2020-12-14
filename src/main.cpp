#include "input_file.h"
#include "lexer.h"
#include "grammar.h"
#include "error.h"
#include "ir.h"
#include "mips.h"

using namespace std;

InputFile input_file("testfile.txt");
ofstream output_file;
ofstream error_file;
ofstream ir_file;
ofstream mips_file;
ofstream mips_file_optim;

Token last_token(0, -1, "");
Token token(0, -1, "");

int main() {
	if (LEXER_OUTPUT || GRAMMAR_OUTPUT) {
		output_file.open("output.txt", ios::out);
	}
	if (ERROR_TYPE_OUTPUT || ERROR_DESCRIPTION_OUTPUT) {
		error_file.open("error.txt", ios::out);
	}
	if (IR_OUTPUT) {
		if (!IR_OPTIMIZE) {
			ir_file.open("testfile_18375362_韩程凯_优化前中间代码.txt", ios::out);
		}
		else {
			ir_file.open("testfile_18375362_韩程凯_优化后中间代码.txt", ios::out);
		}
		
	}
	if (MIPS_CODE_OUTPUT) {
		mips_file.open("mips.txt", ios::out);
		if (!MIPS_CODE_OPTIMIZE) {
			mips_file_optim.open("testfile_18375362_韩程凯_优化前目标代码.txt", ios::out);
		}
		else {
			mips_file_optim.open("testfile_18375362_韩程凯_优化后目标代码.txt", ios::out);
		}
	}
	token = lexer::GetNewToken();
	grammar::program();
	shared_ptr<vector<Quaternion>> ir_vector_ptr = ir::GetQuatVectorPtr();
	ir::IrOptim();
	ir::OutputQuatVector();
	mips::Translate(ir_vector_ptr);
	mips::OutputMipsCodeVec();
}