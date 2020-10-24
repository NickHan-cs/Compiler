#include <fstream>
#include "error.h"

using namespace std;

extern ofstream output_file;

string error::GetErrorTypeNum(ErrorType error_type) {
	return string(1, 'a' + error_type);
}

void error::OutputError(int error_line, ErrorType error_type) {
	if (ERROR_TYPE_OUTPUT) {
		output_file << error_line << " " << error::GetErrorTypeNum(error_type) << endl;
	}
	if (ERROR_DESCRIPTION_OUTPUT) {
	
	}
}