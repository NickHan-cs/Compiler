#pragma once
#include <string>
#include "symbol.h"
#include "ir.h"

const bool GRAMMAR_OUTPUT = false;
const bool GRAMMAR_OUTPUT_DEBUG = false;

namespace grammar {
	Token GetToken();
	void OutputToken(Token token);
	void OutputGrammarElementName(std::string grammar_element_name);
	void MoveStack();
	void ExpectedOrError(int token_sym);
	// 进入新的分程序层次
	void GetIntoNewLevel();
	// 退出当前的分程序层次
	void ExitCurLevel();
	bool is_func_def();
	bool is_var_declaration();
	bool is_var_def_with_init();
	bool is_statement();
	bool is_func_call();
	bool is_func_call_with_ret();
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
	int step();
	void condition_statement();
	OperatorType condition(Operand*&, Operand*&);
	// 如果函数调用返回的是整型，返回INTCON;如果返回的是字符型，则返回CHARCON
	void func_call_with_ret();
	void func_call_no_ret();
	// 返回实参中各个表达式的类型容器
	std::vector<ExprType> value_para_tlb();
	void assign_statement();
	void input_statement();
	void output_statement();
	void case_statement();
	void case_tlb(ExprType, Operand*&, Operand*&, Operand*&);
	void case_substatement(ExprType, Operand*&, Operand*, Operand*, Operand*&);
	void default_statement(Operand*&);
	void ret_statement();
	ExprType expr(Operand *&);
	ExprType term(Operand *&);
	ExprType factor(Operand *&);
	std::string string_sym();
	int const_sym(ExprType);
	int integer();
	int unsigned_integer();
}
