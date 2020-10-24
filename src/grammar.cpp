#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <unordered_set>
#include "Lexer.h"
#include "InputFile.h"
using namespace std;
const bool GRAMMAR_OUTPUT = true;
const bool LEXER_OUTPUT = false;
const bool GRAMMAR_OUTPUT_DEBUG = false;
InputFile input_file("testfile.txt");
ofstream outfile;
Token token(0, -1, "");
stack<Token> token_stack;
stack<Token> temp_token_stack;
unordered_set<string> func_with_ret_set;
unordered_set<string> func_no_ret_set;
namespace grammar {
	void output_sym(Token token);
	void move_stack();
	bool is_func_def();
	bool is_var_declaration();
	bool is_var_def_no_init();
	bool is_var_def_with_init();
	bool is_statement();
	bool is_func_call_with_ret();
	bool is_func_call_no_ret();
	bool is_expr();
	void program();
	void func_def_with_ret();
	void declaration_head();
	void func_def_no_ret();
	void para_tlb();
	void main_func();
	void compound_statements();
	void statement_list();
	void const_declaration();
	void const_def();
	void var_declaration();
	void var_def();
	void var_def_no_init_while();
	void var_def_no_init();
	void var_def_with_init();
	void statement();
	void loop_statement();
	void step();
	void condition_statement();
	void condition();
	void func_call_with_ret();
	void func_call_no_ret();
	void value_para_tlb();
	void assign_statement();
	void input_statement();
	void output_statement();
	void case_statement();
	void case_tlb();
	void case_substatement();
	void default_statement();
	void ret_statement();
	void expr();
	void term();
	void factor();
	void string_sym();
	void const_sym();
	void integer();
	void unsigned_integer();

	void output_sym(Token token) {
		if (GRAMMAR_OUTPUT) {
			outfile << symbol_table[token.get_token_sym()] << " " << token.get_token_str() << endl;
		}
	}

	void move_stack() {
		while (!temp_token_stack.empty()) {
			token_stack.push(temp_token_stack.top());
			temp_token_stack.pop();
		}
	}

	bool is_func_def() {
		if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK &&
			token.get_token_sym() != VOIDTK) {
			return false;
		}
		Token temp_token = Lexer::getsym();
		token_stack.push(temp_token);
		return temp_token.get_token_sym() == IDENFR;
	}

	bool is_var_declaration() {
		if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK) {
			return false;
		}
		temp_token_stack.push(Lexer::getsym());
		if (temp_token_stack.top().get_token_sym() != IDENFR) {
			grammar::move_stack();
			return false;
		}
		Token temp_token = Lexer::getsym();
		temp_token_stack.push(temp_token);
		grammar::move_stack();
		return temp_token.get_token_sym() == LBRACK || temp_token.get_token_sym() == ASSIGN ||
			temp_token.get_token_sym() == COMMA || temp_token.get_token_sym() == SEMICN;
	}

	bool is_var_def_no_init() {
		if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK) {
			return false;
		}
		temp_token_stack.push(Lexer::getsym());
		if (temp_token_stack.top().get_token_sym() != IDENFR) {
			grammar::move_stack();
			return false;
		}
		temp_token_stack.push(Lexer::getsym());
		if (temp_token_stack.top().get_token_sym() == COMMA ||
			temp_token_stack.top().get_token_sym() == SEMICN) {
			grammar::move_stack();
			return true;
		}
		if (temp_token_stack.top().get_token_sym() != LBRACK) {
			grammar::move_stack();
			return false;
		}
		temp_token_stack.push(Lexer::getsym());
		if (temp_token_stack.top().get_token_sym() != INTCON) {
			grammar::move_stack();
			return false;
		}
		temp_token_stack.push(Lexer::getsym());
		if (temp_token_stack.top().get_token_sym() != RBRACK) {
			grammar::move_stack();
			return false;
		}
		temp_token_stack.push(Lexer::getsym());
		if (temp_token_stack.top().get_token_sym() == COMMA ||
			temp_token_stack.top().get_token_sym() == SEMICN) {
			grammar::move_stack();
			return true;
		}
		if (temp_token_stack.top().get_token_sym() != LBRACK) {
			grammar::move_stack();
			return false;
		}
		temp_token_stack.push(Lexer::getsym());
		if (temp_token_stack.top().get_token_sym() != INTCON) {
			grammar::move_stack();
			return false;
		}
		temp_token_stack.push(Lexer::getsym());
		if (temp_token_stack.top().get_token_sym() != RBRACK) {
			grammar::move_stack();
			return false;
		}
		temp_token_stack.push(Lexer::getsym());
		if (temp_token_stack.top().get_token_sym() == COMMA ||
			temp_token_stack.top().get_token_sym() == SEMICN) {
			grammar::move_stack();
			return true;
		}
		grammar::move_stack();
		return false;
	}

	bool is_var_def_with_init() {
		if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK) {
			return false;
		}
		temp_token_stack.push(Lexer::getsym());
		if (temp_token_stack.top().get_token_sym() != IDENFR) {
			grammar::move_stack();
			return false;
		}
		temp_token_stack.push(Lexer::getsym());
		if (temp_token_stack.top().get_token_sym() == ASSIGN) {
			grammar::move_stack();
			return true;
		}
		if (temp_token_stack.top().get_token_sym() != LBRACK) {
			grammar::move_stack();
			return false;
		}
		temp_token_stack.push(Lexer::getsym());
		if (temp_token_stack.top().get_token_sym() != INTCON) {
			grammar::move_stack();
			return false;
		}
		temp_token_stack.push(Lexer::getsym());
		if (temp_token_stack.top().get_token_sym() != RBRACK) {
			grammar::move_stack();
			return false;
		}
		temp_token_stack.push(Lexer::getsym());
		if (temp_token_stack.top().get_token_sym() == ASSIGN) {
			grammar::move_stack();
			return true;
		}
		if (temp_token_stack.top().get_token_sym() != LBRACK) {
			grammar::move_stack();
			return false;
		}
		temp_token_stack.push(Lexer::getsym());
		if (temp_token_stack.top().get_token_sym() != INTCON) {
			grammar::move_stack();
			return false;
		}
		temp_token_stack.push(Lexer::getsym());
		if (temp_token_stack.top().get_token_sym() != RBRACK) {
			grammar::move_stack();
			return false;
		}
		temp_token_stack.push(Lexer::getsym());
		if (temp_token_stack.top().get_token_sym() == ASSIGN) {
			grammar::move_stack();
			return true;
		}
		grammar::move_stack();
		return false;
	}

	bool is_statement() {
		if (token.get_token_sym() == IDENFR) {
			Token temp_token = Lexer::getsym();
			token_stack.push(temp_token);
			return temp_token.get_token_sym() == LPARENT || temp_token.get_token_sym() == ASSIGN ||
				temp_token.get_token_sym() == LBRACK;
		}
		if (GRAMMAR_OUTPUT_DEBUG) {
			outfile << "is_statement " << token.get_token_sym() << endl;
		}
		return token.get_token_sym() == WHILETK || token.get_token_sym() == FORTK || token.get_token_sym() == IFTK ||
			token.get_token_sym() == SCANFTK || token.get_token_sym() == PRINTFTK || token.get_token_sym() == SWITCHTK ||
			token.get_token_sym() == SEMICN || token.get_token_sym() == RETURNTK || token.get_token_sym() == LBRACE;
	}

	bool is_func_call_with_ret() {
		return token.get_token_sym() == IDENFR && 
			func_with_ret_set.find(token.get_token_str()) != func_with_ret_set.end();
	}

	bool is_func_call_no_ret() {
		return token.get_token_sym() == IDENFR &&
			func_no_ret_set.find(token.get_token_str()) != func_no_ret_set.end();
	}

	bool is_expr() {
		return token.get_token_sym() == PLUS || token.get_token_sym() == MINU ||
			token.get_token_sym() == IDENFR || token.get_token_sym() == LPARENT ||
			token.get_token_sym() == INTCON || token.get_token_sym() == CHARCON;
	}

	void program() {
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
			outfile << "<程序>" << endl;
		}
	}

	void func_def_with_ret() {
		grammar::declaration_head();	// 得到函数名
		if (token.get_token_sym() != LPARENT) {
			// error();
			if (GRAMMAR_OUTPUT_DEBUG) {
				outfile << "func_def_with_ret got " << token.get_token_str() << " not LPARENT" << endl;
			}
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
			grammar::para_tlb();
			if (token.get_token_sym() != RPARENT) {
				// error();
				if (GRAMMAR_OUTPUT_DEBUG) {
					outfile << "func_def_with_ret got " << token.get_token_str() << " not RPARENT" << endl;
				}
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
				if (token.get_token_sym() != LBRACE) {
					// error();
					if (GRAMMAR_OUTPUT_DEBUG) {
						outfile << "func_def_with_ret got " << token.get_token_str() << " not LBRACE" << endl;
					}
				}
				else {
					grammar::output_sym(token);
					token = Lexer::getsym();
					grammar::compound_statements();
					if (token.get_token_sym() != RBRACE) {
						// error();
						if (GRAMMAR_OUTPUT_DEBUG) {
							outfile << "func_def_with_ret got " << token.get_token_str() << " not RBRACE" << endl;
						}
					}
					else {
						grammar::output_sym(token);
						token = Lexer::getsym();
					}
				}
			}

		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<有返回值函数定义>" << endl;
		}
	}

	void declaration_head() {
		if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK) {
			// error();
			if (GRAMMAR_OUTPUT_DEBUG) {
				outfile << "declaration_head got " << token.get_token_str() << " not INTTK or CHARTK" << endl;
			}
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != IDENFR) {
				// error();
				if (GRAMMAR_OUTPUT_DEBUG) {
					outfile << "declaration_head got " << token.get_token_str() << " not IDENFR" << endl;
				}
			}
			else {
				grammar::output_sym(token);
				if (token.get_token_str() != "") {
					func_with_ret_set.emplace(token.get_token_str());
				}
				token = Lexer::getsym();
			}
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<声明头部>" << endl;
		}
	}

	void func_def_no_ret() {
		if (token.get_token_sym() != VOIDTK) {
			// error();
			if (GRAMMAR_OUTPUT_DEBUG) {
				outfile << "func_def_no_ret got " << token.get_token_str() << " not VOIDTK" << endl;
			}
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != IDENFR) {
				// error();
				if (GRAMMAR_OUTPUT_DEBUG) {
					outfile << "func_def_no_ret got " << token.get_token_str() << " not IDENFR" << endl;
				}
			}
			else {
				grammar::output_sym(token);
				if (token.get_token_str() != "") {
					func_no_ret_set.emplace(token.get_token_str());
				}
				token = Lexer::getsym();
				if (token.get_token_sym() != LPARENT) {
					// error();
					if (GRAMMAR_OUTPUT_DEBUG) {
						outfile << "func_def_no_ret got " << token.get_token_str() << " not LPARENT" << endl;
					}
				}
				else {
					grammar::output_sym(token);
					token = Lexer::getsym();
					grammar::para_tlb();
					if (token.get_token_sym() != RPARENT) {
						// error();
						if (GRAMMAR_OUTPUT_DEBUG) {
							outfile << "func_def_no_ret got " << token.get_token_str() << " not RPARENT" << endl;
						}
					}
					else {
						grammar::output_sym(token);
						token = Lexer::getsym();
						if (token.get_token_sym() != LBRACE) {
							// error();
							if (GRAMMAR_OUTPUT_DEBUG) {
								outfile << "func_def_no_ret got " << token.get_token_str() << " not LBRACE" << endl;
							}
						}
						else {
							grammar::output_sym(token);
							token = Lexer::getsym();
							grammar::compound_statements();
							if (token.get_token_sym() != RBRACE) {
								// error();
								if (GRAMMAR_OUTPUT_DEBUG) {
									outfile << "func_def_no_ret got " << token.get_token_str() << " not RBRACE" << endl;
								}
							}
							else {
								grammar::output_sym(token);
								token = Lexer::getsym();
							}
						}
					}
				}
			}
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<无返回值函数定义>" << endl;
		}
	}
	
	void para_tlb() {
		if (token.get_token_sym() == INTTK || token.get_token_sym() == CHARTK) {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != IDENFR) {
				// error();
				if (GRAMMAR_OUTPUT_DEBUG) {
					outfile << "para_tlb got " << token.get_token_str() << " not IDENFR" << endl;
				}
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
				while (token.get_token_sym() == COMMA) {
					grammar::output_sym(token);
					token = Lexer::getsym();
					if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK) {
						// error();
						if (GRAMMAR_OUTPUT_DEBUG) {
							outfile << "para_tlb got " << token.get_token_str() << " not INTTK or CHARTK" << endl;
						}
					}
					else {
						grammar::output_sym(token);
						token = Lexer::getsym();
						if (token.get_token_sym() != IDENFR) {
							// error();
							if (GRAMMAR_OUTPUT_DEBUG) {
								outfile << "para_tlb got " << token.get_token_str() << " not IDENFR" << endl;
							}
						}
						else {
							grammar::output_sym(token);
							token = Lexer::getsym();
						}
					}
				}
			}
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<参数表>" << endl;
		}
	}

	void main_func() {
		if (token.get_token_sym() != VOIDTK) {
			// error();
			if (GRAMMAR_OUTPUT_DEBUG) {
				outfile << "main_func got " << token.get_token_str() << " not VOIDTK" << endl;
			}
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != MAINTK) {
				// error();
				if (GRAMMAR_OUTPUT_DEBUG) {
					outfile << "main_func got " << token.get_token_str() << " not MAINTK" << endl;
				}
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
				if (token.get_token_sym() != LPARENT) {
					// error();
					if (GRAMMAR_OUTPUT_DEBUG) {
						outfile << "main_func got " << token.get_token_str() << " not LPARENT" << endl;
					}
				}
				else {
					grammar::output_sym(token);
					token = Lexer::getsym();
					if (token.get_token_sym() != RPARENT) {
						// error();
						if (GRAMMAR_OUTPUT_DEBUG) {
							outfile << "main_func got " << token.get_token_str() << " not RPRAENT" << endl;
						}
					}
					else {
						grammar::output_sym(token);
						token = Lexer::getsym();
						if (token.get_token_sym() != LBRACE) {
							// error();
							if (GRAMMAR_OUTPUT_DEBUG) {
								outfile << "main_func got " << token.get_token_str() << " not LBRACE" << endl;
							}
						}
						else {
							grammar::output_sym(token);
							token = Lexer::getsym();
							grammar::compound_statements();
							if (token.get_token_sym() != RBRACE) {
								// error();
								if (GRAMMAR_OUTPUT_DEBUG) {
									outfile << "main_func got " << token.get_token_str() << " not RBRACE" << endl;
								}
							}
							else {
								grammar::output_sym(token);
								token = Lexer::getsym();
							}
						}
					}
				}
			}
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<主函数>" << endl;
		}
	}

	void compound_statements() {
		if (token.get_token_sym() == CONSTTK) {
			grammar::const_declaration();
		}
		if (token.get_token_sym() == INTTK || token.get_token_sym() == CHARTK) {
			grammar::var_declaration();
		}
		grammar::statement_list();
		if (GRAMMAR_OUTPUT) {
			outfile << "<复合语句>" << endl;
		}
	}

	void statement_list() {
		while (grammar::is_statement()) {
			grammar::statement();
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<语句列>" << endl;
		}
	}

	void const_declaration() {
		if (token.get_token_sym() != CONSTTK) {
			// error();
			if (GRAMMAR_OUTPUT_DEBUG) {
				outfile << "const_declaration got " << token.get_token_str() << " not CONST" << endl;
			}
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
			grammar::const_def();
			if (token.get_token_sym() != SEMICN) {
				// error();
				if (GRAMMAR_OUTPUT_DEBUG) {
					outfile << "const_declaration got " << token.get_token_str() << ", but need a SEMICN to end const_def" << endl;
				}
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
				while (token.get_token_sym() == CONSTTK) {
					grammar::output_sym(token);
					token = Lexer::getsym();
					grammar::const_def();
					if (token.get_token_sym() != SEMICN) {
						// error();
						if (GRAMMAR_OUTPUT_DEBUG) {
							outfile << "const_declaration got " << token.get_token_str() << ", but need a SEMICN to end const_def" << endl;
						}
					}
					else {
						grammar::output_sym(token);
						token = Lexer::getsym();
					}
				}
			}
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<常量说明>" << endl;
		}
	}

	void const_def() {
		if (token.get_token_sym() == INTTK) {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != IDENFR) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
				if (token.get_token_sym() != ASSIGN) {
					// error();
				}
				else {
					grammar::output_sym(token);
					token = Lexer::getsym();
					grammar::integer();
					while (token.get_token_sym() == COMMA) {
						grammar::output_sym(token);
						token = Lexer::getsym();
						if (token.get_token_sym() != IDENFR) {
							// error();
						}
						else {
							grammar::output_sym(token);
							token = Lexer::getsym();
							if (token.get_token_sym() != ASSIGN) {
								// error();
							}
							else {
								grammar::output_sym(token);
								token = Lexer::getsym();
								grammar::integer();
							}
						}
					}
				}
			}
		}
		else if (token.get_token_sym() == CHARTK) {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != IDENFR) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
				if (token.get_token_sym() != ASSIGN) {
					// error();
				}
				else {
					grammar::output_sym(token);
					token = Lexer::getsym();
					if (token.get_token_sym() != CHARCON) {
						// error();
					}
					else {
						grammar::output_sym(token);
						token = Lexer::getsym();
						while (token.get_token_sym() == COMMA) {
							grammar::output_sym(token);
							token = Lexer::getsym();
							if (token.get_token_sym() != IDENFR) {
								// error();
							}
							else {
								grammar::output_sym(token);
								token = Lexer::getsym();
								if (token.get_token_sym() != ASSIGN) {
									// error();
								}
								else {
									grammar::output_sym(token);
									token = Lexer::getsym();
									if (token.get_token_sym() != CHARCON) {
										// error();
									}
									else {
										grammar::output_sym(token);
										token = Lexer::getsym();
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
			outfile << "<常量定义>" << endl;
		}
	}

	void var_declaration() {
		grammar::var_def();
		if (token.get_token_sym() != SEMICN) {
			// error();	
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
		}
		while (grammar::is_var_declaration()) {
			grammar::var_def();
			if (token.get_token_sym() != SEMICN) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
			}
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<变量说明>" << endl;
		}
	}

	void var_def() {
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
			outfile << "<变量定义>" << endl;
		}
	}

	void var_def_no_init_while() {
		while (token.get_token_sym() == COMMA) {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != IDENFR) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
				if (token.get_token_sym() == LBRACK) {
					grammar::output_sym(token);
					token = Lexer::getsym();
					grammar::unsigned_integer();
					if (token.get_token_sym() != RBRACK) {
						// error();
					}
					else {
						grammar::output_sym(token);
						token = Lexer::getsym();
						if (token.get_token_sym() == LBRACK) {
							grammar::output_sym(token);
							token = Lexer::getsym();
							grammar::unsigned_integer();
							if (token.get_token_sym() != RBRACK) {
								// error();
							}
							else {
								grammar::output_sym(token);
								token = Lexer::getsym();
							}
						}
					}
				}
			}
		}
	}

	void var_def_no_init() {
		if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK) {
			// error();
		}
		else {	// <类型标识符>
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != IDENFR) {
				// error();
			}
			else {	// <类型标识符> <标识符>
				grammar::output_sym(token);
				token = Lexer::getsym();
				if (token.get_token_sym() == COMMA) {	// <类型标识符> <标识符>,
					grammar::var_def_no_init_while();
				}
				else if (token.get_token_sym() == LBRACK) {	//<类型标识符> <标识符>[
					grammar::output_sym(token);
					token = Lexer::getsym();
					grammar::unsigned_integer();
					if (token.get_token_sym() != RBRACK) {
						// error();
					}
					else {	// <类型标识符> <标识符>[<无符号整数>]
						grammar::output_sym(token);
						token = Lexer::getsym();
						if (token.get_token_sym() == COMMA) {	// <类型标识符> <标识符>[<无符号整数>],
							grammar::var_def_no_init_while();
						}
						else if (token.get_token_sym() == LBRACK) {	// <类型标识符> <标识符>[<无符号整数>][
							grammar::output_sym(token);
							token = Lexer::getsym();
							grammar::unsigned_integer();
							if (token.get_token_sym() != RBRACK) {
								// error();
							}
							else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]
								grammar::output_sym(token);
								token = Lexer::getsym();
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
			outfile << "<变量定义无初始化>" << endl;
		}
	}

	void var_def_with_init() {
		if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK) {
			// error();
		}
		else {	// <类型标识符>
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != IDENFR) {
				// error();
			}
			else {	// <类型标识符> <标识符>
				grammar::output_sym(token);
				token = Lexer::getsym();
				if (token.get_token_sym() == ASSIGN) {	// <类型标识符> <标识符>=
					grammar::output_sym(token);
					token = Lexer::getsym();
					grammar::const_sym();	// <类型标识符> <标识符>=<常量>
				}
				else if (token.get_token_sym() == LBRACK) {	// <类型标识符> <标识符>[
					grammar::output_sym(token);
					token = Lexer::getsym();
					grammar::unsigned_integer();
					if (token.get_token_sym() != RBRACK) {
						// error();
					}
					else {	// <类型标识符> <标识符>[<无符号整数>]
						grammar::output_sym(token);
						token = Lexer::getsym();
						if (token.get_token_sym() == ASSIGN) {	// <类型标识符> <标识符>[<无符号整数>]=
							grammar::output_sym(token);
							token = Lexer::getsym();
							if (token.get_token_sym() != LBRACE) {
								// error();
							}
							else {	// <类型标识符> <标识符>[<无符号整数>]={
								grammar::output_sym(token);
								token = Lexer::getsym();
								grammar::const_sym();	// <类型标识符> <标识符>[<无符号整数>]={<常量>
								while (token.get_token_sym() == COMMA) {	// <类型标识符> <标识符>[<无符号整数>]={<常量>,{<常量>}
									grammar::output_sym(token);
									token = Lexer::getsym();
									grammar::const_sym();
								}
								if (token.get_token_sym() != RBRACE) {
									// error();
								}
								else {	// <类型标识符> <标识符>[<无符号整数>]={<常量>,{<常量>}
									grammar::output_sym(token);
									token = Lexer::getsym();
								}
							}
						}
						else if (token.get_token_sym() == LBRACK) {	// <类型标识符> <标识符>[<无符号整数>][
							grammar::output_sym(token);
							token = Lexer::getsym();
							grammar::unsigned_integer();
							if (token.get_token_sym() != RBRACK) {
								// error();
							}
							else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]
								grammar::output_sym(token);
								token = Lexer::getsym();
								if (token.get_token_sym() != ASSIGN) {
									// error();
								}
								else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]=
									grammar::output_sym(token);
									token = Lexer::getsym();
									if (token.get_token_sym() != LBRACE) {
										// error();
									}
									else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={
										grammar::output_sym(token);
										token = Lexer::getsym();
										if (token.get_token_sym() != LBRACE) {
											// error();
										}
										else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{
											grammar::output_sym(token);
											token = Lexer::getsym();
											grammar::const_sym();	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{<常量>
											while (token.get_token_sym() == COMMA) {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{<常量>,{<常量>}
												grammar::output_sym(token);
												token = Lexer::getsym();
												grammar::const_sym();
											}
											if (token.get_token_sym() != RBRACE) {
												// error();
											}
											else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{<常量>,{<常量>}}
												grammar::output_sym(token);
												token = Lexer::getsym();
												while (token.get_token_sym() == COMMA) {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3},
													grammar::output_sym(token);
													token = Lexer::getsym();
													if (token.get_token_sym() != LBRACE) {
														// error();
													}
													else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {
														grammar::output_sym(token);
														token = Lexer::getsym();
														grammar::const_sym();	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {4
														while (token.get_token_sym() == COMMA) {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {4,
															grammar::output_sym(token);
															token = Lexer::getsym();
															grammar::const_sym();	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {4, 5
														}
														if (token.get_token_sym() != RBRACE) {
															// error();
														}
														else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {4, 5}
															grammar::output_sym(token);
															token = Lexer::getsym();
														}
													}
												}
												if (token.get_token_sym() != RBRACE) {
													// error();
												}
												else {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {4, 5}, ...}
													grammar::output_sym(token);
													token = Lexer::getsym();
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
			outfile << "<变量定义及初始化>" << endl;
		}
	}

	void statement() {
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
				grammar::output_sym(token);
				token = Lexer::getsym();
			}
		}
		else if (grammar::is_func_call_no_ret()) {
			grammar::func_call_no_ret();
			if (token.get_token_sym() != SEMICN) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
			}
		}
		else if (token.get_token_sym() == IDENFR) {
			grammar::assign_statement();
			if (token.get_token_sym() != SEMICN) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
			}
		}
		else if (token.get_token_sym() == SCANFTK) {
			grammar::input_statement();
			if (token.get_token_sym() != SEMICN) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
			}
		}
		else if (token.get_token_sym() == PRINTFTK) {
			grammar::output_statement();
			if (token.get_token_sym() != SEMICN) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
			}
		}
		else if (token.get_token_sym() == SWITCHTK) {
			grammar::case_statement();
		}
		else if (token.get_token_sym() == SEMICN) {
			grammar::output_sym(token);
			token = Lexer::getsym();
		}
		else if (token.get_token_sym() == RETURNTK) {
			grammar::ret_statement();
			if (token.get_token_sym() != SEMICN) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
			}
		}
		else if (token.get_token_sym() == LBRACE) {
			grammar::output_sym(token);
			token = Lexer::getsym();
			grammar::statement_list();
			if (token.get_token_sym() != RBRACE) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
			}
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<语句>" << endl;
		}
	}

	void loop_statement() {
		if (token.get_token_sym() == WHILETK) {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != LPARENT) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
				grammar::condition();
				if (token.get_token_sym() != RPARENT) {
					// error();
				}
				else {
					grammar::output_sym(token);
					token = Lexer::getsym();
					statement();
				}
			}
		}
		else if (token.get_token_sym() == FORTK) {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != LPARENT) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
				if (token.get_token_sym() != IDENFR) {
					// error();
				}
				else {
					grammar::output_sym(token);
					token = Lexer::getsym();
					if (token.get_token_sym() != ASSIGN) {
						// error();
					}
					else {
						grammar::output_sym(token);
						token = Lexer::getsym();
						grammar::expr();
						if (token.get_token_sym() != SEMICN) {
							// error();
						}
						else {
							grammar::output_sym(token);
							token = Lexer::getsym();
							grammar::condition();
							if (token.get_token_sym() != SEMICN) {
								// error();
							}
							else {
								grammar::output_sym(token);
								token = Lexer::getsym();
								if (token.get_token_sym() != IDENFR) {
									// error();
								}
								else {
									grammar::output_sym(token);
									token = Lexer::getsym();
									if (token.get_token_sym() != ASSIGN) {
										// error();
									}
									else {
										grammar::output_sym(token);
										token = Lexer::getsym();
										if (token.get_token_sym() != IDENFR) {
											// error();
										}
										else {
											grammar::output_sym(token);
											token = Lexer::getsym();
											if (token.get_token_sym() != PLUS && token.get_token_sym() != MINU) {
												// error();
											}
											else {
												grammar::output_sym(token);
												token = Lexer::getsym();
												grammar::step();
												if (token.get_token_sym() != RPARENT) {
													// error();
												}
												else {
													grammar::output_sym(token);
													token = Lexer::getsym();
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
			outfile << "<循环语句>" << endl;
		}
	}

	void step() {
		grammar::unsigned_integer();
		if (GRAMMAR_OUTPUT) {
			outfile << "<步长>" << endl;
		}
	}

	void condition_statement() {
		if (token.get_token_sym() != IFTK) {
			// error();
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != LPARENT) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
				grammar::condition();
				if (token.get_token_sym() != RPARENT) {
					// error();
				}
				else {
					grammar::output_sym(token);
					token = Lexer::getsym();
					grammar::statement();
					if (token.get_token_sym() == ELSETK) {
						grammar::output_sym(token);
						token = Lexer::getsym();
						grammar::statement();
					}
				}
			}
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<条件语句>" << endl;
		}
	}

	void condition() {
		grammar::expr();
		if (token.get_token_sym() != LSS && token.get_token_sym() != LEQ &&
			token.get_token_sym() != GRE && token.get_token_sym() != GEQ &&
			token.get_token_sym() != EQL && token.get_token_sym() != NEQ) {
			// error();
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
			grammar::expr();
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<条件>" << endl;
		}
	}

	void func_call_with_ret() {
		if (token.get_token_sym() != IDENFR) {
			// error();
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != LPARENT) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
				grammar::value_para_tlb();
				if (token.get_token_sym() != RPARENT) {
					//	error();
				}
				else {
					grammar::output_sym(token);
					token = Lexer::getsym();
				}
			}
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<有返回值函数调用语句>" << endl;
		}
	}

	void func_call_no_ret() {
		if (token.get_token_sym() != IDENFR) {
			// error();
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != LPARENT) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
				grammar::value_para_tlb();
				if (token.get_token_sym() != RPARENT) {
					//	error();
				}
				else {
					grammar::output_sym(token);
					token = Lexer::getsym();
				}
			}
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<无返回值函数调用语句>" << endl;
		}
	}

	void value_para_tlb() {
		if (is_expr()) {
			grammar::expr();
			while (token.get_token_sym() == COMMA) {
				grammar::output_sym(token);
				token = Lexer::getsym();
				grammar::expr();
			}
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<值参数表>" << endl;
		}
	}

	void assign_statement() {
		if (token.get_token_sym() != IDENFR) {
			// error();
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() == ASSIGN) {
				grammar::output_sym(token);
				token = Lexer::getsym();
				grammar::expr();
			}
			else if (token.get_token_sym() == LBRACK) {
				grammar::output_sym(token);
				token = Lexer::getsym();
				grammar::expr();
				if (token.get_token_sym() != RBRACK) {
					// error();
				}
				else {
					grammar::output_sym(token);
					token = Lexer::getsym();
					if (token.get_token_sym() == ASSIGN) {
						grammar::output_sym(token);
						token = Lexer::getsym();
						grammar::expr();
					}
					else if (token.get_token_sym() == LBRACK) {
						grammar::output_sym(token);
						token = Lexer::getsym();
						grammar::expr();
						if (token.get_token_sym() != RBRACK) {
							// error();
						}
						else {
							grammar::output_sym(token);
							token = Lexer::getsym();
							if (token.get_token_sym() != ASSIGN) {
								// error();
							}
							else {
								grammar::output_sym(token);
								token = Lexer::getsym();
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
			outfile << "<赋值语句>" << endl;
		}
	}

	void input_statement() {
		if (token.get_token_sym() != SCANFTK) {
			// error();
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != LPARENT) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
				if (token.get_token_sym() != IDENFR) {
					// error();
				}
				else {
					grammar::output_sym(token);
					token = Lexer::getsym();
					if (token.get_token_sym() != RPARENT) {
						// error();
					}
					else {
						grammar::output_sym(token);
						token = Lexer::getsym();
					}
				}
			}
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<读语句>" << endl;
		}
	}

	void output_statement() {
		if (token.get_token_sym() != PRINTFTK) {
			// error();
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != LPARENT) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
				if (token.get_token_sym() == STRCON) {
					grammar::string_sym();
					if (token.get_token_sym() == COMMA) {
						grammar::output_sym(token);
						token = Lexer::getsym();
						grammar::expr();
						if (token.get_token_sym() != RPARENT) {
							// error();
						}
						else {
							grammar::output_sym(token);
							token = Lexer::getsym();
						}
					}
					else if (token.get_token_sym() == RPARENT) {
						grammar::output_sym(token);
						token = Lexer::getsym();
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
						grammar::output_sym(token);
						token = Lexer::getsym();
					}
				}
			}
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<写语句>" << endl;
		}
	}

	void case_statement() {
		if (token.get_token_sym() != SWITCHTK) {
			// error();
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != LPARENT) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
				grammar::expr();
				if (token.get_token_sym() != RPARENT) {
					// error();
				}
				else {
					grammar::output_sym(token);
					token = Lexer::getsym();
					if (token.get_token_sym() != LBRACE) {
						// error();
					}
					else {
						grammar::output_sym(token);
						token = Lexer::getsym();
						grammar::case_tlb();
						grammar::default_statement();
						if (token.get_token_sym() != RBRACE) {
							// error();
						}
						else {
							grammar::output_sym(token);
							token = Lexer::getsym();
						}
					}
				}
			}
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<情况语句>" << endl;
		}
	}

	void case_tlb() {
		grammar::case_substatement();
		while (token.get_token_sym() == CASETK) {
			grammar::case_substatement();
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<情况表>" << endl;
		}
	}

	void case_substatement() {
		if (token.get_token_sym() != CASETK) {
			// error();
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
			grammar::const_sym();
			if (token.get_token_sym() != COLON) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
				grammar::statement();
			}
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<情况子语句>" << endl;
		}
	}

	void default_statement() {
		if (token.get_token_sym() != DEFAULTTK) {
			// error();
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() != COLON) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
				grammar::statement();
			}
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<缺省>" << endl;
		}
	}

	void ret_statement() {
		if (token.get_token_sym() != RETURNTK) {
			// error();
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() == LPARENT) {
				grammar::output_sym(token);
				token = Lexer::getsym();
				grammar::expr();
				if (token.get_token_sym() != RPARENT) {
					// error();
				}
				else {
					grammar::output_sym(token);
					token = Lexer::getsym();
				}
			}
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<返回语句>" << endl;
		}
	}

	void expr() {
		if (token.get_token_sym() == PLUS || token.get_token_sym() == MINU) {
			grammar::output_sym(token);
			token = Lexer::getsym();
		}
		grammar::term();
		while (token.get_token_sym() == PLUS || token.get_token_sym() == MINU) {
			grammar::output_sym(token);
			token = Lexer::getsym();
			grammar::term();
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<表达式>" << endl;
		}
	}

	void term() {
		grammar::factor();
		while (token.get_token_sym() == MULT || token.get_token_sym() == DIV) {
			grammar::output_sym(token);
			token = Lexer::getsym();
			grammar::factor();
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<项>" << endl;
		}
	}

	void factor() {
		if (grammar::is_func_call_with_ret()) {
			grammar::func_call_with_ret();
		}
		else if (token.get_token_sym() == IDENFR) {
			grammar::output_sym(token);
			token = Lexer::getsym();
			if (token.get_token_sym() == LBRACK) {
				grammar::output_sym(token);
				token = Lexer::getsym();
				grammar::expr();
				if (token.get_token_sym() != RBRACK) {
					// error();
				}
				else {
					grammar::output_sym(token);
					token = Lexer::getsym();
					if (token.get_token_sym() == LBRACK) {
						grammar::output_sym(token);
						token = Lexer::getsym();
						grammar::expr();
						if (token.get_token_sym() != RBRACK) {
							// error();
						}
						else {
							grammar::output_sym(token);
							token = Lexer::getsym();
						}
					}
				}
			}
		}
		else if (token.get_token_sym() == LPARENT) {
			grammar::output_sym(token);
			token = Lexer::getsym();
			grammar::expr();
			if (token.get_token_sym() != RPARENT) {
				// error();
			}
			else {
				grammar::output_sym(token);
				token = Lexer::getsym();
			}
		}
		else if (token.get_token_sym() == PLUS || token.get_token_sym() == MINU ||
			token.get_token_sym() == INTCON) {
			grammar::integer();
		}
		else if (token.get_token_sym() == CHARCON) {
			grammar::output_sym(token);
			token = Lexer::getsym();
		}
		else {
			// error();
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<因子>" << endl;
		}
	}

	void string_sym() {
		if (token.get_token_sym() != STRCON) {
			// error();
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<字符串>" << endl;
		}
	}

	void const_sym() {
		if (token.get_token_sym() == CHARCON) {
			grammar::output_sym(token);
			token = Lexer::getsym();
		}
		else {
			integer();
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<常量>" << endl;
		}
	}

	void integer() {
		if (token.get_token_sym() == PLUS || token.get_token_sym() == MINU) {
			grammar::output_sym(token);
			token = Lexer::getsym();
		}
		grammar::unsigned_integer();
		if (GRAMMAR_OUTPUT) {
			outfile << "<整数>" << endl;
		}
	}

	void unsigned_integer() {
		if (token.get_token_sym() != INTCON) {
			// error();
		}
		else {
			grammar::output_sym(token);
			token = Lexer::getsym();
		}
		if (GRAMMAR_OUTPUT) {
			outfile << "<无符号整数>" << endl;
		}
	}
}

int main() {
	if (LEXER_OUTPUT || GRAMMAR_OUTPUT) {
		outfile.open("output.txt", ios::out);
	}
	token = Lexer::getsym();
	grammar::program();
}