#pragma once
#include <string>
#include "symbol.h"

const bool GRAMMAR_OUTPUT = false;
const bool GRAMMAR_OUTPUT_DEBUG = false;

namespace grammar {
	Token GetToken();
	void OutputToken(Token token);
	void OutputGrammarElementName(std::string grammar_element_name);
	void MoveStack();
	void ExpectedOrError(int token_sym);
	// �����µķֳ�����
	void GetIntoNewLevel();

	// �˳���ǰ�ķֳ�����
	void ExitCurLevel();

	bool is_func_def();
	bool is_var_declaration();
	// bool is_var_def_no_init();
	bool is_var_def_with_init();
	bool is_statement();
	bool is_func_call();
	bool is_func_call_with_ret();
	// bool is_func_call_no_ret();
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
	void var_def_no_init_while(int);
	void var_def_no_init();
	void var_def_with_init();
	void statement();
	void loop_statement();
	void step();
	void condition_statement();
	void condition();
	// ����������÷��ص������ͣ�����INTCON;������ص����ַ��ͣ��򷵻�CHARCON
	void func_call_with_ret();
	void func_call_no_ret();
	// ����ʵ���и������ʽ����������
	std::vector<ExprType> value_para_tlb();
	void assign_statement();
	void input_statement();
	void output_statement();
	void case_statement();
	void case_tlb(ExprType);
	void case_substatement(ExprType);
	void default_statement();
	void ret_statement();
	ExprType expr();
	ExprType term();
	ExprType factor();
	void string_sym();
	void const_sym(ExprType);
	void integer();
	int unsigned_integer();
}
