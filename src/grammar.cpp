#include <iostream>
#include <stack>
#include <string>
#include <unordered_set>
#include "lexer.h"
#include "input_file.h"
#include "grammar.h"
#include "error.h"
using namespace std;

extern InputFile input_file;
extern ofstream output_file;
extern Token token;

stack<Token> token_stack;
stack<Token> temp_token_stack;
unordered_set<string> func_with_ret_set;
unordered_set<string> func_no_ret_set;

Token grammar::GetToken() {
	if (!token_stack.empty()) {
		Token token = token_stack.top();
		token_stack.pop();
		return token;
	}
	return lexer::GetNewToken();
}

void grammar::OutputToken(Token token) {
	if (GRAMMAR_OUTPUT) {
		output_file << symbol_table[token.get_token_sym()] << " " << token.get_token_str() << endl;
	}
}

void grammar::MoveStack() {
	while (!temp_token_stack.empty()) {
		token_stack.push(temp_token_stack.top());
		temp_token_stack.pop();
	}
}

bool grammar::is_func_def() {
	if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK &&
		token.get_token_sym() != VOIDTK) {
		return false;
	}
	/*
	Token temp_token = grammar::GetToken();
	token_stack.push(temp_token);
	return temp_token.get_token_sym() == IDENFR;
	*/
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != IDENFR) {
		grammar::MoveStack();
		return false;
	}
	grammar::MoveStack();
	return true;
}

bool grammar::is_var_declaration() {
	if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK) {
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != IDENFR) {
		grammar::MoveStack();
		return false;
	}
	/*
	Token temp_token = grammar::GetToken();
	temp_token_stack.push(temp_token);
	grammar::MoveStack();
	return temp_token.get_token_sym() == LBRACK || temp_token.get_token_sym() == ASSIGN ||
		temp_token.get_token_sym() == COMMA || temp_token.get_token_sym() == SEMICN;
	*/
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != LBRACK && temp_token_stack.top().get_token_sym() != ASSIGN &&
		temp_token_stack.top().get_token_sym() != COMMA && temp_token_stack.top().get_token_sym() != SEMICN) {
		grammar::MoveStack();
		return false;
	}
	grammar::MoveStack();
	return true;
}

bool grammar::is_var_def_no_init() {
	if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK) {
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != IDENFR) {
		grammar::MoveStack();
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() == COMMA ||
		temp_token_stack.top().get_token_sym() == SEMICN) {
		grammar::MoveStack();
		return true;
	}
	if (temp_token_stack.top().get_token_sym() != LBRACK) {
		grammar::MoveStack();
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != INTCON) {
		grammar::MoveStack();
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != RBRACK) {
		grammar::MoveStack();
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() == COMMA ||
		temp_token_stack.top().get_token_sym() == SEMICN) {
		grammar::MoveStack();
		return true;
	}
	if (temp_token_stack.top().get_token_sym() != LBRACK) {
		grammar::MoveStack();
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != INTCON) {
		grammar::MoveStack();
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != RBRACK) {
		grammar::MoveStack();
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != COMMA &&
		temp_token_stack.top().get_token_sym() != SEMICN) {
		grammar::MoveStack();
		return false;
	}
	grammar::MoveStack();
	return true;
}

bool grammar::is_var_def_with_init() {
	if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK) {
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != IDENFR) {
		grammar::MoveStack();
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() == ASSIGN) {
		grammar::MoveStack();
		return true;
	}
	if (temp_token_stack.top().get_token_sym() != LBRACK) {
		grammar::MoveStack();
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != INTCON) {
		grammar::MoveStack();
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != RBRACK) {
		grammar::MoveStack();
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() == ASSIGN) {
		grammar::MoveStack();
		return true;
	}
	if (temp_token_stack.top().get_token_sym() != LBRACK) {
		grammar::MoveStack();
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != INTCON) {
		grammar::MoveStack();
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != RBRACK) {
		grammar::MoveStack();
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != ASSIGN) {
		grammar::MoveStack();
		return false;
	}
	grammar::MoveStack();
	return true;
}

bool grammar::is_statement() {
	if (token.get_token_sym() == IDENFR) {
		/*
		Token temp_token = grammar::GetToken();
		token_stack.push(temp_token);
		return temp_token.get_token_sym() == LPARENT || temp_token.get_token_sym() == ASSIGN ||
			temp_token.get_token_sym() == LBRACK;
		*/
		temp_token_stack.push(grammar::GetToken());
		if (temp_token_stack.top().get_token_sym() != LPARENT && temp_token_stack.top().get_token_sym() != ASSIGN &&
			temp_token_stack.top().get_token_sym() != LBRACK) {
			grammar::MoveStack();
			return false;
		}
		grammar::MoveStack();
		return true;
	}
	if (GRAMMAR_OUTPUT_DEBUG) {
		output_file << "is_statement " << token.get_token_sym() << endl;
	}
	return token.get_token_sym() == WHILETK || token.get_token_sym() == FORTK || token.get_token_sym() == IFTK ||
		token.get_token_sym() == SCANFTK || token.get_token_sym() == PRINTFTK || token.get_token_sym() == SWITCHTK ||
		token.get_token_sym() == SEMICN || token.get_token_sym() == RETURNTK || token.get_token_sym() == LBRACE;
}

bool grammar::is_func_call_with_ret() {
	return token.get_token_sym() == IDENFR &&
		func_with_ret_set.find(token.get_token_str()) != func_with_ret_set.end();
}

bool grammar::is_func_call_no_ret() {
	return token.get_token_sym() == IDENFR &&
		func_no_ret_set.find(token.get_token_str()) != func_no_ret_set.end();
}

bool grammar::is_expr() {
	return token.get_token_sym() == PLUS || token.get_token_sym() == MINU ||
		token.get_token_sym() == IDENFR || token.get_token_sym() == LPARENT ||
		token.get_token_sym() == INTCON || token.get_token_sym() == CHARCON;
}

// <程序>::= [<常量说明>][<变量说明>]{<有返回值函数定义>|<无返回值函数定义>}<主函数>
void grammar::program() {
	if (token.get_token_sym() == CONSTTK) {
		grammar::const_declaration();
	}
	if (grammar::is_var_declaration()) {
		grammar::var_declaration();
	}
	while (grammar::is_func_def()) {
		if (token.get_token_sym() == VOIDTK) {
			grammar::func_def_no_ret();
		}
		else {
			grammar::func_def_with_ret();
		}
	}
	grammar::main_func();
	if (GRAMMAR_OUTPUT) {
		output_file << "<程序>" << endl;
	}
}

// <有返回值函数定义>::=<声明头部>'('<参数表>')''{'<复合语句>'}'
void grammar::func_def_with_ret() {
	grammar::declaration_head();	// 得到函数名
	if (token.get_token_sym() != LPARENT) {
		// error();
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		grammar::para_tlb();
		if (token.get_token_sym() != RPARENT) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			if (token.get_token_sym() != LBRACE) {
				// error();
			}
			else {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				grammar::compound_statements();
				if (token.get_token_sym() != RBRACE) {
					// error();
				}
				else {
					grammar::OutputToken(token);
					token = grammar::GetToken();
				}
			}
		}

	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<有返回值函数定义>" << endl;
	}
}

// <声明头部>::=int<标识符> | char<标识符>
void grammar::declaration_head() {
	if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK) {
		// error();
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != IDENFR) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			if (token.get_token_str() != "") {
				func_with_ret_set.emplace(token.get_token_str());
			}
			token = grammar::GetToken();
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<声明头部>" << endl;
	}
}

// <无返回值函数定义>::=void<标识符>'('<参数表>')''{'<复合语句>'}'
void grammar::func_def_no_ret() {
	if (token.get_token_sym() != VOIDTK) {
		// error();
		if (GRAMMAR_OUTPUT_DEBUG) {
			output_file << "func_def_no_ret got " << token.get_token_str() << " not VOIDTK" << endl;
		}
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != IDENFR) {
			// error();
			if (GRAMMAR_OUTPUT_DEBUG) {
				output_file << "func_def_no_ret got " << token.get_token_str() << " not IDENFR" << endl;
			}
		}
		else {
			grammar::OutputToken(token);
			if (token.get_token_str() != "") {
				func_no_ret_set.emplace(token.get_token_str());
			}
			token = grammar::GetToken();
			if (token.get_token_sym() != LPARENT) {
				// error();
				if (GRAMMAR_OUTPUT_DEBUG) {
					output_file << "func_def_no_ret got " << token.get_token_str() << " not LPARENT" << endl;
				}
			}
			else {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				grammar::para_tlb();
				if (token.get_token_sym() != RPARENT) {
					// error();
					if (GRAMMAR_OUTPUT_DEBUG) {
						output_file << "func_def_no_ret got " << token.get_token_str() << " not RPARENT" << endl;
					}
				}
				else {
					grammar::OutputToken(token);
					token = grammar::GetToken();
					if (token.get_token_sym() != LBRACE) {
						// error();
						if (GRAMMAR_OUTPUT_DEBUG) {
							output_file << "func_def_no_ret got " << token.get_token_str() << " not LBRACE" << endl;
						}
					}
					else {
						grammar::OutputToken(token);
						token = grammar::GetToken();
						grammar::compound_statements();
						if (token.get_token_sym() != RBRACE) {
							// error();
							if (GRAMMAR_OUTPUT_DEBUG) {
								output_file << "func_def_no_ret got " << token.get_token_str() << " not RBRACE" << endl;
							}
						}
						else {
							grammar::OutputToken(token);
							token = grammar::GetToken();
						}
					}
				}
			}
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<无返回值函数定义>" << endl;
	}
}

// <参数表>::=<类型标识符><标识符>{,<类型标识符><标识符>}|<空>
void grammar::para_tlb() {
	if (token.get_token_sym() == INTTK || token.get_token_sym() == CHARTK) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != IDENFR) {
			// error();
			if (GRAMMAR_OUTPUT_DEBUG) {
				output_file << "para_tlb got " << token.get_token_str() << " not IDENFR" << endl;
			}
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			while (token.get_token_sym() == COMMA) {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK) {
					// error();
					if (GRAMMAR_OUTPUT_DEBUG) {
						output_file << "para_tlb got " << token.get_token_str() << " not INTTK or CHARTK" << endl;
					}
				}
				else {
					grammar::OutputToken(token);
					token = grammar::GetToken();
					if (token.get_token_sym() != IDENFR) {
						// error();
						if (GRAMMAR_OUTPUT_DEBUG) {
							output_file << "para_tlb got " << token.get_token_str() << " not IDENFR" << endl;
						}
					}
					else {
						grammar::OutputToken(token);
						token = grammar::GetToken();
					}
				}
			}
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<参数表>" << endl;
	}
}

// <主函数>::=void main '(' ')' '{' <复合语句> '}'
void grammar::main_func() {
	if (token.get_token_sym() != VOIDTK) {
		// error();
		if (GRAMMAR_OUTPUT_DEBUG) {
			output_file << "main_func got " << token.get_token_str() << " not VOIDTK" << endl;
		}
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != MAINTK) {
			// error();
			if (GRAMMAR_OUTPUT_DEBUG) {
				output_file << "main_func got " << token.get_token_str() << " not MAINTK" << endl;
			}
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			if (token.get_token_sym() != LPARENT) {
				// error();
				if (GRAMMAR_OUTPUT_DEBUG) {
					output_file << "main_func got " << token.get_token_str() << " not LPARENT" << endl;
				}
			}
			else {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				if (token.get_token_sym() != RPARENT) {
					// error();
					if (GRAMMAR_OUTPUT_DEBUG) {
						output_file << "main_func got " << token.get_token_str() << " not RPRAENT" << endl;
					}
				}
				else {
					grammar::OutputToken(token);
					token = grammar::GetToken();
					if (token.get_token_sym() != LBRACE) {
						// error();
						if (GRAMMAR_OUTPUT_DEBUG) {
							output_file << "main_func got " << token.get_token_str() << " not LBRACE" << endl;
						}
					}
					else {
						grammar::OutputToken(token);
						token = grammar::GetToken();
						grammar::compound_statements();
						if (token.get_token_sym() != RBRACE) {
							// error();
							if (GRAMMAR_OUTPUT_DEBUG) {
								output_file << "main_func got " << token.get_token_str() << " not RBRACE" << endl;
							}
						}
						else {
							grammar::OutputToken(token);
							token = grammar::GetToken();
						}
					}
				}
			}
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<主函数>" << endl;
	}
}

// <复合语句>::=[<常量说明>][<变量说明>]<语句列>
void grammar::compound_statements() {
	if (token.get_token_sym() == CONSTTK) {
		grammar::const_declaration();
	}
	if (token.get_token_sym() == INTTK || token.get_token_sym() == CHARTK) {
		grammar::var_declaration();
	}
	grammar::statement_list();
	if (GRAMMAR_OUTPUT) {
		output_file << "<复合语句>" << endl;
	}
}

// <语句列>::={<语句>}
void grammar::statement_list() {
	while (grammar::is_statement()) {
		grammar::statement();
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<语句列>" << endl;
	}
}

// <常量说明>::=const<常量定义>;{const<常量定义>;}
void grammar::const_declaration() {
	if (token.get_token_sym() != CONSTTK) {
		// error();
		if (GRAMMAR_OUTPUT_DEBUG) {
			output_file << "const_declaration got " << token.get_token_str() << " not CONST" << endl;
		}
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		grammar::const_def();
		if (token.get_token_sym() != SEMICN) {
			// error();
			if (GRAMMAR_OUTPUT_DEBUG) {
				output_file << "const_declaration got " << token.get_token_str() << ", but need a SEMICN to end const_def" << endl;
			}
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			while (token.get_token_sym() == CONSTTK) {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				grammar::const_def();
				if (token.get_token_sym() != SEMICN) {
					// error();
					if (GRAMMAR_OUTPUT_DEBUG) {
						output_file << "const_declaration got " << token.get_token_str() << ", but need a SEMICN to end const_def" << endl;
					}
				}
				else {
					grammar::OutputToken(token);
					token = grammar::GetToken();
				}
			}
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<常量说明>" << endl;
	}
}

// <常量定义>::=int<标识符>=<整数>{,<标识符>=<整数>} | char <标识符>=<字符>{,<标识符>=<字符>}
void grammar::const_def() {
	if (token.get_token_sym() == INTTK) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != IDENFR) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			if (token.get_token_sym() != ASSIGN) {
				// error();
			}
			else {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				grammar::integer();
				while (token.get_token_sym() == COMMA) {
					grammar::OutputToken(token);
					token = grammar::GetToken();
					if (token.get_token_sym() != IDENFR) {
						// error();
					}
					else {
						grammar::OutputToken(token);
						token = grammar::GetToken();
						if (token.get_token_sym() != ASSIGN) {
							// error();
						}
						else {
							grammar::OutputToken(token);
							token = grammar::GetToken();
							grammar::integer();
						}
					}
				}
			}
		}
	}
	else if (token.get_token_sym() == CHARTK) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != IDENFR) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			if (token.get_token_sym() != ASSIGN) {
				// error();
			}
			else {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				if (token.get_token_sym() != CHARCON) {
					// error();
				}
				else {
					grammar::OutputToken(token);
					token = grammar::GetToken();
					while (token.get_token_sym() == COMMA) {
						grammar::OutputToken(token);
						token = grammar::GetToken();
						if (token.get_token_sym() != IDENFR) {
							// error();
						}
						else {
							grammar::OutputToken(token);
							token = grammar::GetToken();
							if (token.get_token_sym() != ASSIGN) {
								// error();
							}
							else {
								grammar::OutputToken(token);
								token = grammar::GetToken();
								if (token.get_token_sym() != CHARCON) {
									// error();
								}
								else {
									grammar::OutputToken(token);
									token = grammar::GetToken();
								}
							}
						}
					}
				}
			}
		}
	}
	else {
		// error();
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<常量定义>" << endl;
	}
}

// <变量说明>::=<变量定义>;{<变量定义>;}
void grammar::var_declaration() {
	grammar::var_def();
	if (token.get_token_sym() != SEMICN) {
		// error();	
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
	}
	while (grammar::is_var_declaration()) {
		grammar::var_def();
		if (token.get_token_sym() != SEMICN) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<变量说明>" << endl;
	}
}

// <变量定义>::=<变量定义无初始化> | <变量定义及初始化>
void grammar::var_def() {
	if (grammar::is_var_def_with_init()) {
		grammar::var_def_with_init();
	}
	else if (grammar::is_var_def_no_init()) {
		grammar::var_def_no_init();
	}
	else {
		// error();
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<变量定义>" << endl;
	}
}

void grammar::var_def_no_init_while() {
	while (token.get_token_sym() == COMMA) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != IDENFR) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			if (token.get_token_sym() == LBRACK) {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				grammar::unsigned_integer();
				if (token.get_token_sym() != RBRACK) {
					// error();
				}
				else {
					grammar::OutputToken(token);
					token = grammar::GetToken();
					if (token.get_token_sym() == LBRACK) {
						grammar::OutputToken(token);
						token = grammar::GetToken();
						grammar::unsigned_integer();
						if (token.get_token_sym() != RBRACK) {
							// error();
						}
						else {
							grammar::OutputToken(token);
							token = grammar::GetToken();
						}
					}
				}
			}
		}
	}
}

/*
<变量定义无初始化>::=<类型标识符>
					(<标识符>
					| <标识符>'['<无符号整数>']'
					| <标识符>'['<无符号整数>']''['<无符号整数>']')
					{,(<标识符>
					| <标识符>'['<无符号整数>']'
					| <标识符>'['<无符号整数>']''['<无符号整数>']' )}
*/
void grammar::var_def_no_init() {
	if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK) {
		// error();
	}
	else {	// <类型标识符>
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != IDENFR) {
			// error();
		}
		else {	// <类型标识符> <标识符>
			grammar::OutputToken(token);
			token = grammar::GetToken();
			if (token.get_token_sym() == COMMA) {	// <类型标识符> <标识符>,
				grammar::var_def_no_init_while();
			}
			else if (token.get_token_sym() == LBRACK) {	//<类型标识符> <标识符>[
				grammar::OutputToken(token);
				token = grammar::GetToken();
				grammar::unsigned_integer();
				if (token.get_token_sym() != RBRACK) {
					// error();
				}
				else {	// <类型标识符> <标识符>[<无符号整数>]
					grammar::OutputToken(token);
					token = grammar::GetToken();
					if (token.get_token_sym() == COMMA) {	// <类型标识符> <标识符>[<无符号整数>],
						grammar::var_def_no_init_while();
					}
					else if (token.get_token_sym() == LBRACK) {	// <类型标识符> <标识符>[<无符号整数>][
						grammar::OutputToken(token);
						token = grammar::GetToken();
						grammar::unsigned_integer();
						if (token.get_token_sym() != RBRACK) {
							// error();
						}
						else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]
							grammar::OutputToken(token);
							token = grammar::GetToken();
							if (token.get_token_sym() == COMMA) {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>],
								grammar::var_def_no_init_while();
							}
						}
					}
				}
			}
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<变量定义无初始化>" << endl;
	}
}

/*
<变量定义及初始化>::=<类型标识符><标识符>=<常量>
	| <类型标识符><标识符>'['<无符号整数>']'='{'<常量>{,<常量>}'}'
	| <类型标识符><标识符>'['<无符号整数>']''['<无符号整数>']'='{''{'<常量>{,<常量>}'}'{,'{'<常量>{,<常量>}'}'}'}'
*/
void grammar::var_def_with_init() {
	if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK) {
		// error();
	}
	else {	// <类型标识符>
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != IDENFR) {
			// error();
		}
		else {	// <类型标识符> <标识符>
			grammar::OutputToken(token);
			token = grammar::GetToken();
			if (token.get_token_sym() == ASSIGN) {	// <类型标识符> <标识符>=
				grammar::OutputToken(token);
				token = grammar::GetToken();
				grammar::const_sym();	// <类型标识符> <标识符>=<常量>
			}
			else if (token.get_token_sym() == LBRACK) {	// <类型标识符> <标识符>[
				grammar::OutputToken(token);
				token = grammar::GetToken();
				grammar::unsigned_integer();
				if (token.get_token_sym() != RBRACK) {
					// error();
				}
				else {	// <类型标识符> <标识符>[<无符号整数>]
					grammar::OutputToken(token);
					token = grammar::GetToken();
					if (token.get_token_sym() == ASSIGN) {	// <类型标识符> <标识符>[<无符号整数>]=
						grammar::OutputToken(token);
						token = grammar::GetToken();
						if (token.get_token_sym() != LBRACE) {
							// error();
						}
						else {	// <类型标识符> <标识符>[<无符号整数>]={
							grammar::OutputToken(token);
							token = grammar::GetToken();
							grammar::const_sym();	// <类型标识符> <标识符>[<无符号整数>]={<常量>
							while (token.get_token_sym() == COMMA) {	// <类型标识符> <标识符>[<无符号整数>]={<常量>,{<常量>}
								grammar::OutputToken(token);
								token = grammar::GetToken();
								grammar::const_sym();
							}
							if (token.get_token_sym() != RBRACE) {
								// error();
							}
							else {	// <类型标识符> <标识符>[<无符号整数>]={<常量>,{<常量>}
								grammar::OutputToken(token);
								token = grammar::GetToken();
							}
						}
					}
					else if (token.get_token_sym() == LBRACK) {	// <类型标识符> <标识符>[<无符号整数>][
						grammar::OutputToken(token);
						token = grammar::GetToken();
						grammar::unsigned_integer();
						if (token.get_token_sym() != RBRACK) {
							// error();
						}
						else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]
							grammar::OutputToken(token);
							token = grammar::GetToken();
							if (token.get_token_sym() != ASSIGN) {
								// error();
							}
							else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]=
								grammar::OutputToken(token);
								token = grammar::GetToken();
								if (token.get_token_sym() != LBRACE) {
									// error();
								}
								else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={
									grammar::OutputToken(token);
									token = grammar::GetToken();
									if (token.get_token_sym() != LBRACE) {
										// error();
									}
									else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{
										grammar::OutputToken(token);
										token = grammar::GetToken();
										grammar::const_sym();	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{<常量>
										while (token.get_token_sym() == COMMA) {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{<常量>,{<常量>}
											grammar::OutputToken(token);
											token = grammar::GetToken();
											grammar::const_sym();
										}
										if (token.get_token_sym() != RBRACE) {
											// error();
										}
										else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{<常量>,{<常量>}}
											grammar::OutputToken(token);
											token = grammar::GetToken();
											while (token.get_token_sym() == COMMA) {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3},
												grammar::OutputToken(token);
												token = grammar::GetToken();
												if (token.get_token_sym() != LBRACE) {
													// error();
												}
												else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {
													grammar::OutputToken(token);
													token = grammar::GetToken();
													grammar::const_sym();	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {4
													while (token.get_token_sym() == COMMA) {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {4,
														grammar::OutputToken(token);
														token = grammar::GetToken();
														grammar::const_sym();	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {4, 5
													}
													if (token.get_token_sym() != RBRACE) {
														// error();
													}
													else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {4, 5}
														grammar::OutputToken(token);
														token = grammar::GetToken();
													}
												}
											}
											if (token.get_token_sym() != RBRACE) {
												// error();
											}
											else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {4, 5}, ...}
												grammar::OutputToken(token);
												token = grammar::GetToken();
											}
										}
									}
								}
							}
						}
					}
					else {
						//error();
					}
				}
			}
			else {
				// error();
			}
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<变量定义及初始化>" << endl;
	}
}

/*
<语句>::=<循环语句> | <条件语句> | <有返回值函数调用语句>; | <无返回值函数调用语句>; | 
		 <赋值语句>; | <读语句>; | <写语句>; | <情况语句> | <空>; | <返回语句>; | '{'<语句列>'}'
*/
void grammar::statement() {
	if (token.get_token_sym() == WHILETK || token.get_token_sym() == FORTK) {
		grammar::loop_statement();
	}
	else if (token.get_token_sym() == IFTK) {
		grammar::condition_statement();
	}
	else if (grammar::is_func_call_with_ret()) {
		grammar::func_call_with_ret();
		if (token.get_token_sym() != SEMICN) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
		}
	}
	else if (grammar::is_func_call_no_ret()) {
		grammar::func_call_no_ret();
		if (token.get_token_sym() != SEMICN) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
		}
	}
	else if (token.get_token_sym() == IDENFR) {
		grammar::assign_statement();
		if (token.get_token_sym() != SEMICN) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
		}
	}
	else if (token.get_token_sym() == SCANFTK) {
		grammar::input_statement();
		if (token.get_token_sym() != SEMICN) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
		}
	}
	else if (token.get_token_sym() == PRINTFTK) {
		grammar::output_statement();
		if (token.get_token_sym() != SEMICN) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
		}
	}
	else if (token.get_token_sym() == SWITCHTK) {
		grammar::case_statement();
	}
	else if (token.get_token_sym() == SEMICN) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
	}
	else if (token.get_token_sym() == RETURNTK) {
		grammar::ret_statement();
		if (token.get_token_sym() != SEMICN) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
		}
	}
	else if (token.get_token_sym() == LBRACE) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		grammar::statement_list();
		if (token.get_token_sym() != RBRACE) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<语句>" << endl;
	}
}

/*
<循环语句>::=while '('<条件>')'<语句>
		    | for'('<标识符>=<表达式>;<条件>;<标识符>=<标识符>(+|-)<步长>')'<语句>
*/
void grammar::loop_statement() {
	if (token.get_token_sym() == WHILETK) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != LPARENT) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			grammar::condition();
			if (token.get_token_sym() != RPARENT) {
				// error();
			}
			else {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				statement();
			}
		}
	}
	else if (token.get_token_sym() == FORTK) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != LPARENT) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			if (token.get_token_sym() != IDENFR) {
				// error();
			}
			else {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				if (token.get_token_sym() != ASSIGN) {
					// error();
				}
				else {
					grammar::OutputToken(token);
					token = grammar::GetToken();
					grammar::expr();
					if (token.get_token_sym() != SEMICN) {
						// error();
					}
					else {
						grammar::OutputToken(token);
						token = grammar::GetToken();
						grammar::condition();
						if (token.get_token_sym() != SEMICN) {
							// error();
						}
						else {
							grammar::OutputToken(token);
							token = grammar::GetToken();
							if (token.get_token_sym() != IDENFR) {
								// error();
							}
							else {
								grammar::OutputToken(token);
								token = grammar::GetToken();
								if (token.get_token_sym() != ASSIGN) {
									// error();
								}
								else {
									grammar::OutputToken(token);
									token = grammar::GetToken();
									if (token.get_token_sym() != IDENFR) {
										// error();
									}
									else {
										grammar::OutputToken(token);
										token = grammar::GetToken();
										if (token.get_token_sym() != PLUS && token.get_token_sym() != MINU) {
											// error();
										}
										else {
											grammar::OutputToken(token);
											token = grammar::GetToken();
											grammar::step();
											if (token.get_token_sym() != RPARENT) {
												// error();
											}
											else {
												grammar::OutputToken(token);
												token = grammar::GetToken();
												grammar::statement();
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else {
		// error();
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<循环语句>" << endl;
	}
}

// <步长>::=<无符号整数>
void grammar::step() {
	grammar::unsigned_integer();
	if (GRAMMAR_OUTPUT) {
		output_file << "<步长>" << endl;
	}
}

// <条件语句>::=if'('<条件>')'<语句>[else<语句>]
void grammar::condition_statement() {
	if (token.get_token_sym() != IFTK) {
		// error();
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != LPARENT) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			grammar::condition();
			if (token.get_token_sym() != RPARENT) {
				// error();
			}
			else {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				grammar::statement();
				if (token.get_token_sym() == ELSETK) {
					grammar::OutputToken(token);
					token = grammar::GetToken();
					grammar::statement();
				}
			}
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<条件语句>" << endl;
	}
}

// <条件>::=<表达式><关系运算符><表达式>
void grammar::condition() {
	grammar::expr();
	if (token.get_token_sym() != LSS && token.get_token_sym() != LEQ &&
		token.get_token_sym() != GRE && token.get_token_sym() != GEQ &&
		token.get_token_sym() != EQL && token.get_token_sym() != NEQ) {
		// error();
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		grammar::expr();
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<条件>" << endl;
	}
}

// <有返回值函数调用语句>::=<标识符>'('<值参数表>')'
void grammar::func_call_with_ret() {
	if (token.get_token_sym() != IDENFR) {
		// error();
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != LPARENT) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			grammar::value_para_tlb();
			if (token.get_token_sym() != RPARENT) {
				//	error();
			}
			else {
				grammar::OutputToken(token);
				token = grammar::GetToken();
			}
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<有返回值函数调用语句>" << endl;
	}
}

// <无返回值函数调用语句>::=<标识符>'('<值参数表>')'
void grammar::func_call_no_ret() {
	if (token.get_token_sym() != IDENFR) {
		// error();
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != LPARENT) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			grammar::value_para_tlb();
			if (token.get_token_sym() != RPARENT) {
				//	error();
			}
			else {
				grammar::OutputToken(token);
				token = grammar::GetToken();
			}
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<无返回值函数调用语句>" << endl;
	}
}

// <值参数表>::=<表达式>{,<表达式>} | <空>
void grammar::value_para_tlb() {
	if (grammar::is_expr()) {
		grammar::expr();
		while (token.get_token_sym() == COMMA) {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			grammar::expr();
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<值参数表>" << endl;
	}
}

/*
<赋值语句>::=<标识符>=<表达式>
			| <标识符>'['<表达式>']'=<表达式>
			| <标识符>'['<表达式>']''['<表达式>']'=<表达式>
*/
void grammar::assign_statement() {
	if (token.get_token_sym() != IDENFR) {
		// error();
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() == ASSIGN) {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			grammar::expr();
		}
		else if (token.get_token_sym() == LBRACK) {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			grammar::expr();
			if (token.get_token_sym() != RBRACK) {
				// error();
			}
			else {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				if (token.get_token_sym() == ASSIGN) {
					grammar::OutputToken(token);
					token = grammar::GetToken();
					grammar::expr();
				}
				else if (token.get_token_sym() == LBRACK) {
					grammar::OutputToken(token);
					token = grammar::GetToken();
					grammar::expr();
					if (token.get_token_sym() != RBRACK) {
						// error();
					}
					else {
						grammar::OutputToken(token);
						token = grammar::GetToken();
						if (token.get_token_sym() != ASSIGN) {
							// error();
						}
						else {
							grammar::OutputToken(token);
							token = grammar::GetToken();
							grammar::expr();
						}
					}
				}
				else {
					// error();
				}
			}
		}
		else {
			// error();
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<赋值语句>" << endl;
	}
}

// <读语句>::=scanf'('<标识符>')'
void grammar::input_statement() {
	if (token.get_token_sym() != SCANFTK) {
		// error();
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != LPARENT) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			if (token.get_token_sym() != IDENFR) {
				// error();
			}
			else {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				if (token.get_token_sym() != RPARENT) {
					// error();
				}
				else {
					grammar::OutputToken(token);
					token = grammar::GetToken();
				}
			}
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<读语句>" << endl;
	}
}

// <写语句>::=printf'('<字符串>,<表达式>')' | printf'('<字符串>')' | printf'('<表达式>')'
void grammar::output_statement() {
	if (token.get_token_sym() != PRINTFTK) {
		// error();
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != LPARENT) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			if (token.get_token_sym() == STRCON) {
				grammar::string_sym();
				if (token.get_token_sym() == COMMA) {
					grammar::OutputToken(token);
					token = grammar::GetToken();
					grammar::expr();
					if (token.get_token_sym() != RPARENT) {
						// error();
					}
					else {
						grammar::OutputToken(token);
						token = grammar::GetToken();
					}
				}
				else if (token.get_token_sym() == RPARENT) {
					grammar::OutputToken(token);
					token = grammar::GetToken();
				}
				else {
					// error();
				}
			}
			else {
				grammar::expr();
				if (token.get_token_sym() != RPARENT) {
					// error();
				}
				else {
					grammar::OutputToken(token);
					token = grammar::GetToken();
				}
			}
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<写语句>" << endl;
	}
}

// <情况语句>::=switch '('<表达式>')' '{'<情况表><缺省>'}'
void grammar::case_statement() {
	if (token.get_token_sym() != SWITCHTK) {
		// error();
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != LPARENT) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			grammar::expr();
			if (token.get_token_sym() != RPARENT) {
				// error();
			}
			else {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				if (token.get_token_sym() != LBRACE) {
					// error();
				}
				else {
					grammar::OutputToken(token);
					token = grammar::GetToken();
					grammar::case_tlb();
					grammar::default_statement();
					if (token.get_token_sym() != RBRACE) {
						// error();
					}
					else {
						grammar::OutputToken(token);
						token = grammar::GetToken();
					}
				}
			}
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<情况语句>" << endl;
	}
}

// <情况表>::=<情况子语句>{<情况子语句>}
void grammar::case_tlb() {
	grammar::case_substatement();
	while (token.get_token_sym() == CASETK) {
		grammar::case_substatement();
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<情况表>" << endl;
	}
}

// <情况子语句>::=case <常量>: <语句>
void grammar::case_substatement() {
	if (token.get_token_sym() != CASETK) {
		// error();
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		grammar::const_sym();
		if (token.get_token_sym() != COLON) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			grammar::statement();
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<情况子语句>" << endl;
	}
}

// <缺省>::=default: <语句>
void grammar::default_statement() {
	if (token.get_token_sym() != DEFAULTTK) {
		// error();
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != COLON) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			grammar::statement();
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<缺省>" << endl;
	}
}

// <返回语句>::=return['('<表达式>')']
void grammar::ret_statement() {
	if (token.get_token_sym() != RETURNTK) {
		// error();
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() == LPARENT) {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			grammar::expr();
			if (token.get_token_sym() != RPARENT) {
				// error();
			}
			else {
				grammar::OutputToken(token);
				token = grammar::GetToken();
			}
		}
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<返回语句>" << endl;
	}
}

// <表达式>::=[+|-]<项>{<加法运算符><项>}
void grammar::expr() {
	if (token.get_token_sym() == PLUS || token.get_token_sym() == MINU) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
	}
	grammar::term();
	while (token.get_token_sym() == PLUS || token.get_token_sym() == MINU) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		grammar::term();
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<表达式>" << endl;
	}
}

// <项>::=<因子>{<乘法运算符><因子>}
void grammar::term() {
	grammar::factor();
	while (token.get_token_sym() == MULT || token.get_token_sym() == DIV) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		grammar::factor();
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<项>" << endl;
	}
}

/*
<因子>::=<标识符>
		| <标识符>'['<表达式>']'
		| <标识符>'['<表达式>']''['<表达式>']'
		| '('<表达式>')'
		| <整数>
		| <字符>
		| <有返回值函数调用语句>
*/
void grammar::factor() {
	if (grammar::is_func_call_with_ret()) {
		grammar::func_call_with_ret();
	}
	else if (token.get_token_sym() == IDENFR) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() == LBRACK) {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			grammar::expr();
			if (token.get_token_sym() != RBRACK) {
				// error();
			}
			else {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				if (token.get_token_sym() == LBRACK) {
					grammar::OutputToken(token);
					token = grammar::GetToken();
					grammar::expr();
					if (token.get_token_sym() != RBRACK) {
						// error();
					}
					else {
						grammar::OutputToken(token);
						token = grammar::GetToken();
					}
				}
			}
		}
	}
	else if (token.get_token_sym() == LPARENT) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		grammar::expr();
		if (token.get_token_sym() != RPARENT) {
			// error();
		}
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
		}
	}
	else if (token.get_token_sym() == PLUS || token.get_token_sym() == MINU ||
		token.get_token_sym() == INTCON) {
		grammar::integer();
	}
	else if (token.get_token_sym() == CHARCON) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
	}
	else {
		// 有可能出现无返回值函数调用语句
		// error();
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<因子>" << endl;
	}
}

// <字符串>::="{十进制编码为32,33,35-126的ASCII字符}"
void grammar::string_sym() {
	if (token.get_token_sym() != STRCON) {
		// error();
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<字符串>" << endl;
	}
}

// <常量>::=<整数> | <字符>
void grammar::const_sym() {
	if (token.get_token_sym() == CHARCON) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
	}
	else {
		integer();
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<常量>" << endl;
	}
}

// <整数>::=[+|-]<无符号整数>
void grammar::integer() {
	if (token.get_token_sym() == PLUS || token.get_token_sym() == MINU) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
	}
	grammar::unsigned_integer();
	if (GRAMMAR_OUTPUT) {
		output_file << "<整数>" << endl;
	}
}

// <无符号整数>::= <数字>{<数字>}
void grammar::unsigned_integer() {
	if (token.get_token_sym() != INTCON) {
		// error();
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
	}
	if (GRAMMAR_OUTPUT) {
		output_file << "<无符号整数>" << endl;
	}
}