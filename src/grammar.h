#pragma once

const bool GRAMMAR_OUTPUT = true;
const bool GRAMMAR_OUTPUT_DEBUG = false;

namespace grammar {
	Token GetToken();
	void OutputToken(Token token);
	void MoveStack();
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
}

