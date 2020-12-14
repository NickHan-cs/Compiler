#include <iostream>
#include <stack>
#include <unordered_set>
#include <cassert>
#include "lexer.h"
#include "grammar.h"
#include "symbol.h"
#include "error.h"
#include "ir.h"
using namespace std;

extern ofstream output_file;
extern Token last_token;
extern Token token;
static int label_idx = 0;;

stack<Token> token_stack;
stack<Token> temp_token_stack;

static int subprogram_level = -1;	// 分程序层次
stack<int> subprogram_level_stack;	// 分程序层次栈

const StringOperand enter_string_opreand_ptr = StringOperand("enter_str", "\\n");
const ImmeOperand zero_imme_operand_ptr = ImmeOperand(0);
const SymbolOperand v0_symbol_operand_ptr = SymbolOperand("$v0");

enum ReturnState {
	InNoRetFunc,				// 在无返回值函数定义
	InIntRetFuncNoReturn,		// 在返回整型的函数定义中且无return
	InCharRetFuncNoReturn,		// 在返回字符型的函数定义且无return
	InIntRetFuncWithReturn,		// 在返回整型的函数定义中且已有return
	InCharRetFuncWithReturn,	// 在返回字符型的函数定义中且已有return
	NotInFunc					// 不在函数定义
};

ReturnState return_state = NotInFunc;

string ProdLabelName() {
	label_idx += 1;
	return "Label_" + to_string(label_idx);
}

Token grammar::GetToken() {
	// 保存前一个token，这里的token是全局变量
	last_token = token;
	if (!token_stack.empty()) {
		Token token = token_stack.top();
		token_stack.pop();
		return token;
	}
	return lexer::GetNewToken();
}

void grammar::OutputToken(Token token) {
	if (GRAMMAR_OUTPUT) {
		output_file << token_table[token.get_token_sym()] << " " << token.get_token_str() << endl;
	}
}

void grammar::OutputGrammarElementName(string grammar_element_name) {
	if (GRAMMAR_OUTPUT) {
		output_file << grammar_element_name << endl;
	}
}

void grammar::MoveStack() {
	while (!temp_token_stack.empty()) {
		token_stack.push(temp_token_stack.top());
		temp_token_stack.pop();
	}
}

void grammar::ExpectedOrError(int token_sym) {
	if (token.get_token_sym() == token_sym) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		return;
	}
	if (token_sym == SEMICN) {
		// 应为分号;
		assert(last_token.get_line() <= token.get_line());
		error::OutputError(last_token.get_line(), SemicnMiss);
	}
	else if (token_sym == RPARENT) {
		// 应为右小括号')'
		error::OutputError(token.get_line(), RparentMiss);
	}
	else if (token_sym == RBRACK) {
		// 应为右中括号']'
		error::OutputError(token.get_line(), RbrackMiss);
	}
	else if (token_sym == CHARCON || token_sym == CONSTTK || token_sym == VOIDTK || token_sym == MAINTK || 
		token_sym == IFTK || token_sym == SWITCHTK || token_sym == CASETK || token_sym == DEFAULTTK || token_sym == SCANFTK || 
		token_sym == PRINTFTK || token_sym == RETURNTK || token_sym == COLON || token_sym == ASSIGN || token_sym == LPARENT || 
		token_sym == LBRACK || token_sym == LBRACE || token_sym == RBRACE) {
		/*
		token_sym == CHARCON 应为字符常量
		token_sym == VOIDTK 应为void
		token_sym == MAINTK 应为main
		token_sym == IFTK 应为if
		token_sym == SWITCHTK 应为switch
		token_sym == SCANFTK 应为scanf
		token_sym == PRINTFTK 应为printf
		token_sym == COLON 应为:
		token_sym == ASSIGN 应为=
		token_sym == LPARENT 应为'('
		token_sym == LBRACK 应为'['
		token_sym == LBRACE	应为'{'
		token_sym == RBRACE	应为'}'
		*/
		// error::OutputError(token.get_line(), OtherError);
	}
}

void grammar::GetIntoNewLevel() {
	subprogram_level += 1;	// 分程序的层次加1
	subprogram_level_stack.push(subprogram_level);
}

void grammar::ExitCurLevel() {
	// 需要弹出subprogram_level_stack的第一项，并将该level的所有标识符从symbol_table中清空
	int cur_level = subprogram_level_stack.top();
	subprogram_level_stack.pop();
	symbol_table::DelCurLevelSymbol(cur_level);
}

bool grammar::is_func_def() {
	if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK &&
		token.get_token_sym() != VOIDTK) {
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != IDENFR) {
		grammar::MoveStack();
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != LPARENT) {
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
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() == LPARENT) {
		// 不是函数定义，就默认为变量声明
		grammar::MoveStack();
		return false;
	}
	grammar::MoveStack();
	return true;
}

bool grammar::is_var_def_with_init() {
	temp_token_stack.push(grammar::GetToken());
	while (temp_token_stack.top().get_token_sym() != INTTK && temp_token_stack.top().get_token_sym() != CHARTK &&
		temp_token_stack.top().get_token_sym() != VOIDTK && temp_token_stack.top().get_token_sym() != COMMA &&
		temp_token_stack.top().get_token_sym() != SEMICN) {
		if (temp_token_stack.top().get_token_sym() == ASSIGN) {
			grammar::MoveStack();
			return true;
		}
		temp_token_stack.push(grammar::GetToken());
	}
	grammar::MoveStack();
	return false;
}

bool grammar::is_statement() {
	if (token.get_token_sym() == IDENFR) {
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

bool grammar::is_func_call() {
	if (token.get_token_sym() != IDENFR) {
		return false;
	}
	temp_token_stack.push(grammar::GetToken());
	if (temp_token_stack.top().get_token_sym() != LPARENT) {
		grammar::MoveStack();
		return false;
	}
	grammar::MoveStack();
	return true;
}

bool grammar::is_func_call_with_ret() {
	if (!grammar::is_func_call()) {
		return false;
	}
	return symbol_table::IsWithRetFunc(token.get_token_str());
}

bool grammar::is_expr() {
	return token.get_token_sym() == PLUS || token.get_token_sym() == MINU ||
		token.get_token_sym() == IDENFR || token.get_token_sym() == LPARENT ||
		token.get_token_sym() == INTCON || token.get_token_sym() == CHARCON;
}

// <程序>::= [<常量说明>][<变量说明>]{<有返回值函数定义>|<无返回值函数定义>}<主函数>
void grammar::program() {
	grammar::GetIntoNewLevel();
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
	grammar::ExitCurLevel();
	grammar::OutputGrammarElementName("<程序>");
}

// <有返回值函数定义>::=<声明头部>'('<参数表>')''{'<复合语句>'}'
void grammar::func_def_with_ret() {
	grammar::declaration_head();	// 得到函数名
	grammar::GetIntoNewLevel();
	ExpectedOrError(LPARENT);
	grammar::para_tlb();
	ExpectedOrError(RPARENT);
	ExpectedOrError(LBRACE);
	grammar::compound_statements();
	// 没有出现return
	if (return_state == InIntRetFuncNoReturn || return_state == InCharRetFuncNoReturn) {
		error::OutputError(token.get_line(), RetFuncMissReturn);
	}
	ExpectedOrError(RBRACE);
	grammar::ExitCurLevel();
	return_state = NotInFunc;
	grammar::OutputGrammarElementName("<有返回值函数定义>");
}

// <声明头部>::=int<标识符> | char<标识符>
void grammar::declaration_head() {
	// 如果声明头部中没有声明函数返回值类型，默认该函数为无返回值函数
	SymbolType func_ret_type = VoidFuncSym;
	if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK) {
		// 声明头部中没有声明返回值类型 或 返回值类型不是int或char
		// error();
	}
	else {
		if (token.get_token_sym() == INTTK) {
			func_ret_type = IntFuncSym;
			return_state = InIntRetFuncNoReturn;
		}
		else if (token.get_token_sym() == CHARTK) {
			func_ret_type = CharFuncSym;
			return_state = InCharRetFuncNoReturn;
		}
		grammar::OutputToken(token);
		token = grammar::GetToken();
	}
	if (token.get_token_sym() != IDENFR) {
		// 声明头部中没有指定函数名
		// error();
	}
	else {
		string func_name = token.get_token_str();
		if (!symbol_table::AddFuncSymbol(func_name, func_ret_type, subprogram_level_stack.top())) {
			error::OutputError(token.get_line(), NameRedefine);
		}
		Operand* func_label_operand_ptr = new LabelOperand("_" + func_name + "_Label");
		ir::AddQuaternionBack(Quaternion(OperatorType::LABEL, func_label_operand_ptr));
		grammar::OutputToken(token);
		token = grammar::GetToken();
	}
	grammar::OutputGrammarElementName("<声明头部>");
}

// <无返回值函数定义>::=void<标识符>'('<参数表>')''{'<复合语句>'}'
void grammar::func_def_no_ret() {
	return_state = InNoRetFunc;
	ExpectedOrError(VOIDTK);
	if (token.get_token_sym() != IDENFR) {
		// 无返回值函数声明时没有声明函数名
		// error();
	}
	else {
		string func_name = token.get_token_str();
		if (!symbol_table::AddFuncSymbol(func_name, VoidFuncSym, subprogram_level_stack.top())) {
			error::OutputError(token.get_line(), NameRedefine);
		}
		Operand* func_label_operand_ptr = new LabelOperand("_" + func_name + "_Label");
		ir::AddQuaternionBack(Quaternion(OperatorType::LABEL, func_label_operand_ptr));
		grammar::OutputToken(token);
		token = grammar::GetToken();
	}
	grammar::GetIntoNewLevel();
	ExpectedOrError(LPARENT);
	grammar::para_tlb();
	ExpectedOrError(RPARENT);
	ExpectedOrError(LBRACE);
	grammar::compound_statements();
	ExpectedOrError(RBRACE);
	grammar::ExitCurLevel();
	return_state = NotInFunc;
	grammar::OutputGrammarElementName("<无返回值函数定义>");
}

// <参数表>::=<类型标识符><标识符>{,<类型标识符><标识符>}|<空>
void grammar::para_tlb() {
	shared_ptr<Symbol> symbol_ptr = symbol_table::GetSymbolPtrVectorBack();
	shared_ptr<FuncSymbol> func_symbol_ptr = NULL;
	if (symbol_table::IsFuncSymbol(symbol_ptr->get_symbol_type())) {
		func_symbol_ptr = dynamic_pointer_cast<FuncSymbol>(symbol_ptr);
	}
	if (token.get_token_sym() == INTTK || token.get_token_sym() == CHARTK) {
		SymbolType var_symbol_type = token.get_token_sym() == CHARTK ? CharVarSym : IntVarSym;
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != IDENFR) {
			// 参数表中int 或 char后没有标识符
			// error();
		}
		else {
			if (!symbol_table::AddVarSymbol(token.get_token_str(), var_symbol_type, subprogram_level_stack.top())) {
				error::OutputError(token.get_line(), NameRedefine);
			} 
			else if (func_symbol_ptr) {
				shared_ptr<Symbol> arg_symbol_ptr = symbol_table::GetSymbolPtrVectorBack();
				ir::AddQuaternionBack(Quaternion(OperatorType::PARAM, ir::GetSymbolOperandPtr(arg_symbol_ptr)));
				assert(symbol_table::IsVarSymbol(arg_symbol_ptr->get_symbol_type()));
				shared_ptr<VarSymbol> var_symbol_ptr = dynamic_pointer_cast<VarSymbol>(arg_symbol_ptr);
				func_symbol_ptr->add_func_symbol_args_ptr(var_symbol_ptr);
			}
			grammar::OutputToken(token);
			token = grammar::GetToken();
		}
		while (token.get_token_sym() == COMMA) {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK) {
				// 参数表中,后不是int 或 char，如果是这样后面的标识符也不读了，因为无法判断标识符类型
				// error();
			}
			else {
				var_symbol_type = token.get_token_sym() == CHARTK ? CharVarSym : IntVarSym;
				grammar::OutputToken(token);
				token = grammar::GetToken();
				if (token.get_token_sym() != IDENFR) {
					// 参数表中int 或 char后没有标识符
					// error();
				}
				else {
					if (!symbol_table::AddVarSymbol(token.get_token_str(), var_symbol_type, subprogram_level_stack.top())) {
						error::OutputError(token.get_line(), NameRedefine);
					}
					else if (func_symbol_ptr) {
						shared_ptr<Symbol> arg_symbol_ptr = symbol_table::GetSymbolPtrVectorBack();
						ir::AddQuaternionBack(Quaternion(OperatorType::PARAM, ir::GetSymbolOperandPtr(arg_symbol_ptr)));
						assert(symbol_table::IsVarSymbol(arg_symbol_ptr->get_symbol_type()));
						shared_ptr<VarSymbol> var_symbol_ptr = dynamic_pointer_cast<VarSymbol>(arg_symbol_ptr);
						func_symbol_ptr->add_func_symbol_args_ptr(var_symbol_ptr);
					}
					grammar::OutputToken(token);
					token = grammar::GetToken();
				}
			}
		}
	}
	grammar::OutputGrammarElementName("<参数表>");
}

// <主函数>::=void main '(' ')' '{' <复合语句> '}'
void grammar::main_func() {
	Operand* main_label_operand_ptr = new LabelOperand("_main_Label");
	ir::AddQuaternionBack(Quaternion(LABEL, main_label_operand_ptr));
	return_state = InNoRetFunc;
	ExpectedOrError(VOIDTK);
	ExpectedOrError(MAINTK);
	ExpectedOrError(LPARENT);
	ExpectedOrError(RPARENT);
	ExpectedOrError(LBRACE);
	grammar::GetIntoNewLevel();
	grammar::compound_statements();
	grammar::ExitCurLevel();
	ExpectedOrError(RBRACE);
	grammar::OutputGrammarElementName("<主函数>");
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
	grammar::OutputGrammarElementName("<复合语句>");
}

// <语句列>::={<语句>}
void grammar::statement_list() {
	while (grammar::is_statement()) {
		grammar::statement();
	}
	grammar::OutputGrammarElementName("<语句列>");
}

// <常量说明>::=const<常量定义>;{const<常量定义>;}
void grammar::const_declaration() {
	ExpectedOrError(CONSTTK);
	grammar::const_def();
	ExpectedOrError(SEMICN);
	while (token.get_token_sym() == CONSTTK) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		grammar::const_def();
		ExpectedOrError(SEMICN);
	}
	grammar::OutputGrammarElementName("<常量说明>");
}

// <常量定义>::=int<标识符>=<整数>{,<标识符>=<整数>} | char <标识符>=<字符>{,<标识符>=<字符>}
void grammar::const_def() {
	if (token.get_token_sym() == INTTK) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		string const_name = token.get_token_str();

		if (token.get_token_sym() != IDENFR) {
			// int后没有标识符
			// error();
		}
		else {
			if (!symbol_table::AddVarSymbol(const_name, IntConSym, subprogram_level_stack.top())) {
				error::OutputError(token.get_line(), NameRedefine);
			}
			grammar::OutputToken(token);
			token = grammar::GetToken();
		}

		ExpectedOrError(ASSIGN);

		int const_value = grammar::integer();
		shared_ptr<Symbol> symbol_ptr = symbol_table::GetSymbolLatest(const_name);
		if (symbol_ptr->get_symbol_type() == IntConSym) {
			shared_ptr<VarSymbol> const_symbol_ptr = dynamic_pointer_cast<VarSymbol>(symbol_ptr);
			const_symbol_ptr->set_var_symbol_value(const_value);
		}

		while (token.get_token_sym() == COMMA) {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			const_name = token.get_token_str();
			
			if (token.get_token_sym() != IDENFR) {
				// ,后面没有标识符
				// error();
			}
			else {
				if (!symbol_table::AddVarSymbol(const_name, IntConSym, subprogram_level_stack.top())) {
					error::OutputError(token.get_line(), NameRedefine);
				}
				grammar::OutputToken(token);
				token = grammar::GetToken();
			}
			
			ExpectedOrError(ASSIGN);
			
			const_value = grammar::integer();
			shared_ptr<Symbol> symbol_ptr = symbol_table::GetSymbolLatest(const_name);
			if (symbol_ptr->get_symbol_type() == IntConSym) {
				shared_ptr<VarSymbol> const_symbol_ptr = dynamic_pointer_cast<VarSymbol>(symbol_ptr);
				const_symbol_ptr->set_var_symbol_value(const_value);
			}
		}
	}
	else if (token.get_token_sym() == CHARTK) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		string const_name = token.get_token_str();
		
		if (token.get_token_sym() != IDENFR) {
			// ,后面没有标识符
			// error();
		}
		else {
			if (!symbol_table::AddVarSymbol(const_name, CharConSym, subprogram_level_stack.top())) {
				error::OutputError(token.get_line(), NameRedefine);
			}
			grammar::OutputToken(token);
			token = grammar::GetToken();
		}
		
		ExpectedOrError(ASSIGN);
		
		int const_value = 0;
		if (token.get_token_sym() != CHARCON) {
			// error();
		}
		else {
			const_value = token.get_token_str()[0];
			shared_ptr<Symbol> symbol_ptr = symbol_table::GetSymbolLatest(const_name);
			if (symbol_ptr->get_symbol_type() == CharConSym) {
				shared_ptr<VarSymbol> const_symbol_ptr = dynamic_pointer_cast<VarSymbol>(symbol_ptr);
				const_symbol_ptr->set_var_symbol_value(const_value);
			}
			
			grammar::OutputToken(token);
			token = grammar::GetToken();
		}
		while (token.get_token_sym() == COMMA) {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			const_name = token.get_token_str();
			
			if (token.get_token_sym() != IDENFR) {
				// ,后面没有标识符
				// error();
			}
			else {
				if (!symbol_table::AddVarSymbol(const_name, CharConSym, subprogram_level_stack.top())) {
					error::OutputError(token.get_line(), NameRedefine);
				}
				grammar::OutputToken(token);
				token = grammar::GetToken();
			}
			
			ExpectedOrError(ASSIGN);
			
			const_value = 0;
			if (token.get_token_sym() != CHARCON) {
				// error();
			}
			else {
				const_value = token.get_token_str()[0];
				shared_ptr<Symbol> symbol_ptr = symbol_table::GetSymbolLatest(const_name);
				if (symbol_ptr->get_symbol_type() == CharConSym) {
					shared_ptr<VarSymbol> const_symbol_ptr = dynamic_pointer_cast<VarSymbol>(symbol_ptr);
					const_symbol_ptr->set_var_symbol_value(const_value);
				}
				grammar::OutputToken(token);
				token = grammar::GetToken();
			}
		}
	}
	else {
		// const后既不是int也不是char，无法判断类型，是不是应该跳读至;
		// error();
	}
	grammar::OutputGrammarElementName("<常量定义>");
}

// <变量说明>::=<变量定义>;{<变量定义>;}
void grammar::var_declaration() {
	grammar::var_def();
	ExpectedOrError(SEMICN);
	while (grammar::is_var_declaration()) {
		grammar::var_def();
		ExpectedOrError(SEMICN);
	}
	grammar::OutputGrammarElementName("<变量说明>");
}

// <变量定义>::=<变量定义无初始化> | <变量定义及初始化>
void grammar::var_def() {
	if (grammar::is_var_def_with_init()) {
		grammar::var_def_with_init();
	}
	else {
		grammar::var_def_no_init();
	}
	grammar::OutputGrammarElementName("<变量定义>");
}

void grammar::var_def_no_init_while(int symbol_type_sym) {
	while (token.get_token_sym() == COMMA) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != IDENFR) {
			// ','后面没有标识符
			// error();
		}
		else {
			string symbol_name = token.get_token_str();
			int symbol_line = token.get_line();
			SymbolType symbol_type = symbol_type_sym == CHARTK ? CharVarSym : IntVarSym;
			grammar::OutputToken(token);
			token = grammar::GetToken();
			if (token.get_token_sym() == LBRACK) {
				symbol_type = symbol_type_sym == CHARTK ? CharArrSym : IntArrSym;
				grammar::OutputToken(token);
				token = grammar::GetToken();
				int arr_symbol_dim_1 = grammar::unsigned_integer();
				int arr_symbol_dim_2 = -1;
				/*
				这边可能存在报错先后问题，如果遇到名字重定义且']'缺失的情况，
				理论上应该先报名字重定义，再报']'缺失，
				但是如果该程序运行是先报']'缺失，再报名字重定义。
				不过由于无恶意换行且单行只有一个错误的限制，不会出现上述情况，不过之后可以略微修改。
				*/
				ExpectedOrError(RBRACK);
				if (token.get_token_sym() == LBRACK) {
					grammar::OutputToken(token);
					token = grammar::GetToken();
					arr_symbol_dim_2 = grammar::unsigned_integer();
					ExpectedOrError(RBRACK);
				}
				if (!symbol_table::AddArrSymbol(symbol_name, symbol_type, subprogram_level_stack.top(), arr_symbol_dim_1, arr_symbol_dim_2)) {
					error::OutputError(symbol_line, NameRedefine);
				}
				Operand* dim_1_operand_ptr = new ImmeOperand(arr_symbol_dim_1);
				if (arr_symbol_dim_2 == -1) {
					ir::AddQuaternionBack(Quaternion(OperatorType::ARR_VAR, ir::GetSymbolOperandPtr(symbol_table::GetSymbolLatest(symbol_name)), dim_1_operand_ptr));
				}
				else {
					Operand* dim_2_operand_ptr = new ImmeOperand(arr_symbol_dim_2);
					ir::AddQuaternionBack(Quaternion(OperatorType::ARR_VAR, ir::GetSymbolOperandPtr(symbol_table::GetSymbolLatest(symbol_name)), dim_1_operand_ptr, dim_2_operand_ptr));
				}
			}
			else {
				if (!symbol_table::AddVarSymbol(symbol_name, symbol_type, subprogram_level_stack.top())) {
					// 已经读到标识符的下一个token，所以last_token才是标识符
					error::OutputError(last_token.get_line(), NameRedefine);
				}
				ir::AddQuaternionBack(Quaternion(VAR, ir::GetSymbolOperandPtr(symbol_table::GetSymbolLatest(symbol_name))));
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
		// 变量定义初始化开头不是int或char，似乎不会出现这种情况，因为都是经过预读的
		// error();
	}
	else {	
		// <类型标识符>
		int symbol_type_sym = token.get_token_sym();
		grammar::OutputToken(token);
		token = grammar::GetToken();

		if (token.get_token_sym() != IDENFR) {
			// int 或 char没有标识符，应该跳读至,或;
			// error();
		}
		else {	
			// <类型标识符> <标识符>
			string symbol_name = token.get_token_str();
			int symbol_line = token.get_line();
			grammar::OutputToken(token);
			token = grammar::GetToken();

			if (token.get_token_sym() == COMMA) {
				// <类型标识符> <标识符>,
				// 读到','说明一个变量定义无初始化结束了，应该加入到符号表中
				SymbolType symbol_type = symbol_type_sym == CHARTK ? CharVarSym : IntVarSym;
				if (!symbol_table::AddVarSymbol(symbol_name, symbol_type, subprogram_level_stack.top())) {
					error::OutputError(symbol_line, NameRedefine);
				}
				ir::AddQuaternionBack(Quaternion(VAR, ir::GetSymbolOperandPtr(symbol_table::GetSymbolLatest(symbol_name))));
				
				grammar::var_def_no_init_while(symbol_type_sym);
			}
			else if (token.get_token_sym() == LBRACK) {	
				//<类型标识符> <标识符>[
				SymbolType symbol_type = symbol_type_sym == CHARTK ? CharArrSym : IntArrSym;
				grammar::OutputToken(token);
				token = grammar::GetToken();

				int arr_symbol_dim_1 = grammar::unsigned_integer();

				ExpectedOrError(RBRACK);	

				// <类型标识符> <标识符>[<无符号整数>]
				if (token.get_token_sym() == COMMA) {	
					// <类型标识符> <标识符>[<无符号整数>],
					if (!symbol_table::AddArrSymbol(symbol_name, symbol_type, subprogram_level_stack.top(), arr_symbol_dim_1, -1)) {
						error::OutputError(symbol_line, NameRedefine);
					}
					Operand* dim_1_operand_ptr = new ImmeOperand(arr_symbol_dim_1);
					ir::AddQuaternionBack(Quaternion(OperatorType::ARR_VAR, ir::GetSymbolOperandPtr(symbol_table::GetSymbolLatest(symbol_name)), dim_1_operand_ptr));
					
					grammar::var_def_no_init_while(symbol_type_sym);
				}
				else if (token.get_token_sym() == LBRACK) {	
					// <类型标识符> <标识符>[<无符号整数>][
					grammar::OutputToken(token);
					token = grammar::GetToken();
					int arr_symbol_dim_2 = grammar::unsigned_integer();

					ExpectedOrError(RBRACK);

					// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]
					if (!symbol_table::AddArrSymbol(symbol_name, symbol_type, subprogram_level_stack.top(), arr_symbol_dim_1, arr_symbol_dim_2)) {
						error::OutputError(symbol_line, NameRedefine);
					}
					Operand* dim_1_operand_ptr = new ImmeOperand(arr_symbol_dim_1);
					Operand* dim_2_operand_ptr = new ImmeOperand(arr_symbol_dim_2);
					ir::AddQuaternionBack(Quaternion(OperatorType::ARR_VAR, ir::GetSymbolOperandPtr(symbol_table::GetSymbolLatest(symbol_name)), dim_1_operand_ptr, dim_2_operand_ptr));
					
					if (token.get_token_sym() == COMMA) {	
						// <类型标识符> <标识符>[<无符号整数>][<无符号整数>],
						grammar::var_def_no_init_while(symbol_type_sym);
					}
				}
				else {
					// <类型标识符> <标识符>[<无符号整数>];
					if (!symbol_table::AddArrSymbol(symbol_name, symbol_type, subprogram_level_stack.top(), arr_symbol_dim_1, -1)) {
						error::OutputError(symbol_line, NameRedefine);
					}
					Operand* dim_1_operand_ptr = new ImmeOperand(arr_symbol_dim_1);
					ir::AddQuaternionBack(Quaternion(OperatorType::ARR_VAR, ir::GetSymbolOperandPtr(symbol_table::GetSymbolLatest(symbol_name)), dim_1_operand_ptr));
				}
			}
			else {
				// <类型标识符> <标识符>;
				SymbolType symbol_type = symbol_type_sym == CHARTK ? CharVarSym : IntVarSym;
				if (!symbol_table::AddVarSymbol(symbol_name, symbol_type, subprogram_level_stack.top())) {
					error::OutputError(symbol_line, NameRedefine);
				}
				ir::AddQuaternionBack(Quaternion(VAR, ir::GetSymbolOperandPtr(symbol_table::GetSymbolLatest(symbol_name))));
			}
		}
	}
	grammar::OutputGrammarElementName("<变量定义无初始化>");
}

/*
<变量定义及初始化>::=<类型标识符><标识符>=<常量>
	| <类型标识符><标识符>'['<无符号整数>']'='{'<常量>{,<常量>}'}'
	| <类型标识符><标识符>'['<无符号整数>']''['<无符号整数>']'='{''{'<常量>{,<常量>}'}'{,'{'<常量>{,<常量>}'}'}'}'
*/
void grammar::var_def_with_init() {
	if (token.get_token_sym() != INTTK && token.get_token_sym() != CHARTK) {
		// 该情况不会出现，因为进入<变量定义及初始化>需要预读
	}
	else {	
		// <类型标识符>
		int symbol_type_sym = token.get_token_sym();
		ExprType expr_type = symbol_type_sym == CHARTK ? CharExpr : IntExpr;
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() != IDENFR) {
			// 该情况不会出现，因为进入<变量定义及初始化>需要预读
		}
		else {	
			// <类型标识符> <标识符>
			string symbol_name = token.get_token_str();
			int symbol_line = token.get_line();
			grammar::OutputToken(token);
			token = grammar::GetToken();
			if (token.get_token_sym() == ASSIGN) {	
				// <类型标识符> <标识符>=
				// 即使在'='后面出现了问题，该变量还是被加入了符号表
				SymbolType symbol_type = symbol_type_sym == CHARTK ? CharVarSym : IntVarSym;
				if (!symbol_table::AddVarSymbol(symbol_name, symbol_type, subprogram_level_stack.top())) {
					error::OutputError(symbol_line, NameRedefine);
				}
				Operand* symbol_operand_ptr = ir::GetSymbolOperandPtr(symbol_table::GetSymbolLatest(symbol_name));
				ir::AddQuaternionBack(Quaternion(OperatorType::VAR, symbol_operand_ptr));
				grammar::OutputToken(token);
				token = grammar::GetToken();
				int instant_value = grammar::const_sym(expr_type);	// <类型标识符> <标识符>=<常量>
				Operand* imme_operand_ptr = new ImmeOperand(instant_value);
				ir::AddQuaternionBack(Quaternion(OperatorType::LOAD_IMM, symbol_operand_ptr, imme_operand_ptr));
			}
			else if (token.get_token_sym() == LBRACK) {	
				// <类型标识符> <标识符>'['
				SymbolType symbol_type = symbol_type_sym == CHARTK ? CharArrSym : IntArrSym;
				grammar::OutputToken(token);
				token = grammar::GetToken();
				int arr_symbol_dim_1 = grammar::unsigned_integer();
				Operand* dim_1_operand_ptr = new ImmeOperand(arr_symbol_dim_1);
				// 此处也可能有报错先后问题
				ExpectedOrError(RBRACK);	// <类型标识符> <标识符>'['<无符号整数>']'
				if (token.get_token_sym() == ASSIGN) {	
					// <类型标识符> <标识符>'['<无符号整数>']'=
					if (!symbol_table::AddArrSymbol(symbol_name, symbol_type, subprogram_level_stack.top(), arr_symbol_dim_1, -1)) {
						error::OutputError(symbol_line, NameRedefine);
					}
					Operand* symbol_operand_ptr = ir::GetSymbolOperandPtr(symbol_table::GetSymbolLatest(symbol_name));
					ir::AddQuaternionBack(Quaternion(OperatorType::ARR_VAR, symbol_operand_ptr, dim_1_operand_ptr));
					grammar::OutputToken(token);
					token = grammar::GetToken();
					ExpectedOrError(LBRACE);	// <类型标识符> <标识符>'['<无符号整数>']'='{'
					int instant_num_1 = 0;
					Operand* instant_num_operand_ptr = new ImmeOperand(instant_num_1);
					int instant_value = grammar::const_sym(expr_type);	// <类型标识符> <标识符>'['<无符号整数>']'='{'<常量>
					Operand* imme_operand_ptr = new ImmeOperand(instant_value);
					ir::AddQuaternionBack(Quaternion(OperatorType::ARR_INIT, imme_operand_ptr, symbol_operand_ptr, instant_num_operand_ptr));
					instant_num_1 += 1;
					while (token.get_token_sym() == COMMA) {	
						// <类型标识符> <标识符>'['<无符号整数>']'='{'<常量>,{<常量>}
						grammar::OutputToken(token);
						token = grammar::GetToken();
						instant_value = grammar::const_sym(expr_type);
						Operand* imme_operand_ptr = new ImmeOperand(instant_value);
						Operand* instant_num_operand_ptr = new ImmeOperand(instant_num_1);
						ir::AddQuaternionBack(Quaternion(OperatorType::ARR_INIT, imme_operand_ptr, symbol_operand_ptr, instant_num_operand_ptr));
						instant_num_1 += 1;
					}
					if (instant_num_1 != arr_symbol_dim_1) {
						error::OutputError(token.get_line(), ArrInitNotMatch);
					}
					ExpectedOrError(RBRACE);	// <类型标识符> <标识符>'['<无符号整数>']'='{'<常量>,{<常量>}'}'
				}
				else if (token.get_token_sym() == LBRACK) {	
					// <类型标识符> <标识符>'['<无符号整数>']''['
					grammar::OutputToken(token);
					token = grammar::GetToken();
					int arr_symbol_dim_2 = grammar::unsigned_integer();
					Operand* dim_2_operand_ptr = new ImmeOperand(arr_symbol_dim_2);
					// 此处也可能有报错先后问题
					ExpectedOrError(RBRACK);	// <类型标识符> <标识符>'['<无符号整数>']''['<无符号整数>']'
					if (!symbol_table::AddArrSymbol(symbol_name, symbol_type, subprogram_level_stack.top(), arr_symbol_dim_1, arr_symbol_dim_2)) {
						error::OutputError(symbol_line, NameRedefine);
					}
					Operand* symbol_operand_ptr = ir::GetSymbolOperandPtr(symbol_table::GetSymbolLatest(symbol_name));
					ir::AddQuaternionBack(Quaternion(OperatorType::ARR_VAR, symbol_operand_ptr, dim_1_operand_ptr, dim_2_operand_ptr));
					ExpectedOrError(ASSIGN);	// <类型标识符> <标识符>'['<无符号整数>']''['<无符号整数>']'=
					ExpectedOrError(LBRACE);	// <类型标识符> <标识符>'['<无符号整数>']''['<无符号整数>']'='{'
					int instant_num_1 = 0;
					ExpectedOrError(LBRACE);	// <类型标识符> <标识符>'['<无符号整数>']''['<无符号整数>']'='{''{'
					instant_num_1 += 1;
					int instant_num_2 = 0;
					int array_idx = 0;
					Operand* array_idx_operand_ptr = new ImmeOperand(array_idx);
					int instant_value = grammar::const_sym(expr_type);	// <类型标识符> <标识符>'['<无符号整数>']''['<无符号整数>']'='{''{'<常量>
					Operand* imme_operand_ptr = new ImmeOperand(instant_value);
					ir::AddQuaternionBack(Quaternion(OperatorType::ARR_INIT, imme_operand_ptr, symbol_operand_ptr, array_idx_operand_ptr));
					instant_num_2 += 1;
					while (token.get_token_sym() == COMMA) {	// <类型标识符> <标识符>'['<无符号整数>']''['<无符号整数>']'='{''{'<常量>,{<常量>}
						grammar::OutputToken(token);
						token = grammar::GetToken();
						array_idx += 1;
						Operand* array_idx_operand_ptr = new ImmeOperand(array_idx);
						instant_value = grammar::const_sym(expr_type);
						Operand* imme_operand_ptr = new ImmeOperand(instant_value);
						ir::AddQuaternionBack(Quaternion(OperatorType::ARR_INIT, imme_operand_ptr, symbol_operand_ptr, array_idx_operand_ptr));
						instant_num_2 += 1;
					}
					if (instant_num_2 != arr_symbol_dim_2) {
						error::OutputError(token.get_line(), ArrInitNotMatch);
					}
					ExpectedOrError(RBRACE);	// <类型标识符> <标识符>'['<无符号整数>']''['<无符号整数>']'='{''{'<常量>,{<常量>}'}'
					while (token.get_token_sym() == COMMA) {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3},
						grammar::OutputToken(token);
						token = grammar::GetToken();
						ExpectedOrError(LBRACE);	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {
						instant_num_1 += 1;
						instant_num_2 = 0;
						array_idx += 1;
						Operand* array_idx_operand_ptr = new ImmeOperand(array_idx);
						instant_value = grammar::const_sym(expr_type);	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {4
						Operand* imme_operand_ptr = new ImmeOperand(instant_value);
						ir::AddQuaternionBack(Quaternion(OperatorType::ARR_INIT, imme_operand_ptr, symbol_operand_ptr, array_idx_operand_ptr));
						instant_num_2 += 1;
						while (token.get_token_sym() == COMMA) {	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {4,
							grammar::OutputToken(token);
							token = grammar::GetToken();
							array_idx += 1;
							Operand* array_idx_operand_ptr = new ImmeOperand(array_idx);
							instant_value = grammar::const_sym(expr_type);	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {4, 5
							Operand* imme_operand_ptr = new ImmeOperand(instant_value);
							ir::AddQuaternionBack(Quaternion(OperatorType::ARR_INIT, imme_operand_ptr, symbol_operand_ptr, array_idx_operand_ptr));
							instant_num_2 += 1;
						}
						if (instant_num_2 != arr_symbol_dim_2) {
							error::OutputError(token.get_line(), ArrInitNotMatch);
						}
						ExpectedOrError(RBRACE);	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {4, 5}
					}
					if (instant_num_1 != arr_symbol_dim_1) {
						error::OutputError(token.get_line(), ArrInitNotMatch);
					}
					ExpectedOrError(RBRACE);	// <类型标识符> <标识符>[<无符号整数>][<无符号整数>]={{1, 2, 3}, {4, 5}, ...}
				}
				else {
					// 该情况不会出现，因为进入<变量定义及初始化>需要预读
				}
			}
			else {
				// 该情况不会出现，因为进入<变量定义及初始化>需要预读
			}
		}
	}
	grammar::OutputGrammarElementName("<变量定义及初始化>");
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
	else if (grammar::is_func_call()) {
		/*
		如果符号表中能查到标识符确实是有返回值函数，
		就是<有返回函数调用语句>，否则就是<无返回值函数调用语句>；
		如果实际上这个标识符没有被定义，在<无返回值函数调用语句>，就直接跳过这句话到分号
		*/
		if (grammar::is_func_call_with_ret()) {
			grammar::func_call_with_ret();
		}
		else {
			grammar::func_call_no_ret();
		}
		ExpectedOrError(SEMICN);
	}
	else if (token.get_token_sym() == IDENFR) {
		grammar::assign_statement();
		ExpectedOrError(SEMICN);
	}
	else if (token.get_token_sym() == SCANFTK) {
		grammar::input_statement();
		ExpectedOrError(SEMICN);
	}
	else if (token.get_token_sym() == PRINTFTK) {
		grammar::output_statement();
		ExpectedOrError(SEMICN);
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
		ExpectedOrError(SEMICN);
	}
	else if (token.get_token_sym() == LBRACE) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		grammar::statement_list();
		ExpectedOrError(RBRACE);
	}
	grammar::OutputGrammarElementName("<语句>");
}

/*
<循环语句>::=while '('<条件>')'<语句>
		    | for'('<标识符>=<表达式>;<条件>;<标识符>=<标识符>(+|-)<步长>')'<语句>
*/
void grammar::loop_statement() {
	if (token.get_token_sym() == WHILETK) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		ExpectedOrError(LPARENT);
		Operand* entry_label = new LabelOperand(ProdLabelName());
		// ENTRY_LABEL:
		ir::AddQuaternionBack(Quaternion(LABEL, entry_label));
		Operand *loperand_ptr = nullptr, *roperand_ptr = nullptr;
		// 得到条件语句的关系运算符和左右操作符
		OperatorType condition_operator = grammar::condition(loperand_ptr, roperand_ptr);
		// BNE EXIT_LABEL, $t0, $t1
		Operand* exit_label = new LabelOperand(ProdLabelName());
		ir::AddQuaternionBack(Quaternion(condition_operator, exit_label, loperand_ptr, roperand_ptr));
		ExpectedOrError(RPARENT);
		// <语句>
		grammar::statement();
		// GOTO ENTRY_LABEL
		ir::AddQuaternionBack(Quaternion(GOTO, entry_label));
		// EXIT_LABEL:
		ir::AddQuaternionBack(Quaternion(LABEL, exit_label));
	}
	else if (token.get_token_sym() == FORTK) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		ExpectedOrError(LPARENT);
		string symbol_name = "";
		if (token.get_token_sym() != IDENFR) {
			// error();
		}
		else {
			symbol_name = token.get_token_str();
			int symbol_line = token.get_line();
			if (!symbol_table::IsNameDefined(symbol_name)) {
				error::OutputError(symbol_line, NameNotDefine);
			}
			if (symbol_table::IsConSymbol(symbol_table::GetSymbolLatestType(symbol_name))) {
				error::OutputError(symbol_line, ModifyConst);
			}
			grammar::OutputToken(token);
			token = grammar::GetToken();
		}
		ExpectedOrError(ASSIGN);
		Operand* symbol_operand_ptr = ir::GetSymbolOperandPtr(symbol_table::GetSymbolLatest(symbol_name));
		Operand* expr_operand_ptr = nullptr;
		grammar::expr(expr_operand_ptr);
		// 可以进行优化，直接将expr中最后一个四元式dest_ptr换成symbol_operand_ptr，就可以不move了
		ir::AddQuaternionBack(Quaternion(MV, symbol_operand_ptr, expr_operand_ptr));
		ExpectedOrError(SEMICN);
		// ENTRY_LABEL:
		Operand* entry_label = new LabelOperand(ProdLabelName());
		ir::AddQuaternionBack(Quaternion(LABEL, entry_label));
		Operand *loperand_ptr = nullptr, *roperand_ptr = nullptr;
		// 得到条件语句的关系运算符和左右操作符
		OperatorType condition_operator = grammar::condition(loperand_ptr, roperand_ptr);
		ExpectedOrError(SEMICN);
		// BNE EXIT_LABEL, $t0, $t1
		Operand* exit_label = new LabelOperand(ProdLabelName());
		ir::AddQuaternionBack(Quaternion(condition_operator, exit_label, loperand_ptr, roperand_ptr));
		
		string step_dest_operand_name = "";
		Operand* step_dest_operand_ptr = nullptr;
		if (token.get_token_sym() != IDENFR) {
			// error();
		}
		else {
			step_dest_operand_name = token.get_token_str();
			int symbol_line = token.get_line();
			if (!symbol_table::IsNameDefined(step_dest_operand_name)) {
				error::OutputError(symbol_line, NameNotDefine);
			}
			if (symbol_table::IsConSymbol(symbol_table::GetSymbolLatestType(step_dest_operand_name))) {
				error::OutputError(symbol_line, ModifyConst);
			}
			step_dest_operand_ptr = ir::GetSymbolOperandPtr(symbol_table::GetSymbolLatest(step_dest_operand_name));
			grammar::OutputToken(token);
			token = grammar::GetToken();
		}
		ExpectedOrError(ASSIGN);
		string step_loperand_name = "";
		Operand* step_loperand_ptr = nullptr;
		if (token.get_token_sym() != IDENFR) {
			// error();
		}
		else {
			step_loperand_name = token.get_token_str();
			if (!symbol_table::IsNameDefined(step_loperand_name)) {
				error::OutputError(token.get_line(), NameNotDefine);
			}
			shared_ptr<Symbol> symbol_ptr = symbol_table::GetSymbolLatest(step_loperand_name);
			if (symbol_table::IsConSymbol(symbol_ptr->get_symbol_type())) {
				shared_ptr<VarSymbol> var_symbol_ptr = dynamic_pointer_cast<VarSymbol>(symbol_ptr);
				step_loperand_ptr = new ImmeOperand(var_symbol_ptr->get_var_symbol_value());
			}
			else {
				step_loperand_ptr = ir::GetSymbolOperandPtr(symbol_table::GetSymbolLatest(step_loperand_name));
			}
			grammar::OutputToken(token);
			token = grammar::GetToken();
		}
		OperatorType operator_type = OperatorType::ADDU;
		if (token.get_token_sym() != PLUS && token.get_token_sym() != MINU) {
			// error();
		}
		else {
			operator_type = token.get_token_sym() == PLUS ? OperatorType::ADDU : OperatorType::SUBU;
			grammar::OutputToken(token);
			token = grammar::GetToken();
		}
		int step_value = grammar::step();
		Operand* step_roperand_ptr = new ImmeOperand(step_value);
		Quaternion step_quat = Quaternion(operator_type, step_dest_operand_ptr, step_loperand_ptr, step_roperand_ptr);
		ExpectedOrError(RPARENT);
		grammar::statement();
		// 步长操作
		ir::AddQuaternionBack(step_quat);
		// GOTO ENTRY_LABEL
		ir::AddQuaternionBack(Quaternion(GOTO, entry_label));
		// EXIT_LABEL:
		ir::AddQuaternionBack(Quaternion(LABEL, exit_label));
	}
	else {
		// 这种情况应该不会发生，因为是经过预读进入<循环语句>的
		// error();
	}
	grammar::OutputGrammarElementName("<循环语句>");
}

// <步长>::=<无符号整数>
int grammar::step() {
	int value = grammar::unsigned_integer();
	grammar::OutputGrammarElementName("<步长>");
	return value;
}

// <条件语句>::=if'('<条件>')'<语句>[else<语句>]
void grammar::condition_statement() {
	ExpectedOrError(IFTK);
	ExpectedOrError(LPARENT);
	Operand *loperand_ptr = nullptr, *roperand_ptr = nullptr;
	OperatorType condition_operator = grammar::condition(loperand_ptr, roperand_ptr);
	Operand *false_label = new LabelOperand(ProdLabelName());
	ir::AddQuaternionBack(Quaternion(condition_operator, false_label, loperand_ptr, roperand_ptr));
	ExpectedOrError(RPARENT);
	grammar::statement();
	if (token.get_token_sym() == ELSETK) {
		Operand *exit_label = new LabelOperand(ProdLabelName());
		ir::AddQuaternionBack(Quaternion(GOTO, exit_label));
		ir::AddQuaternionBack(Quaternion(LABEL, false_label));
		grammar::OutputToken(token);
		token = grammar::GetToken();
		grammar::statement();
		ir::AddQuaternionBack(Quaternion(LABEL, exit_label));
	}
	else {
		ir::AddQuaternionBack(Quaternion(LABEL, false_label));
	}
	grammar::OutputGrammarElementName("<条件语句>");
}

// <条件>::=<表达式><关系运算符><表达式>
OperatorType grammar::condition(Operand *&loperand_ptr, Operand*& roperand_ptr) {
	int expr_line = token.get_line();
	if (grammar::expr(loperand_ptr) != IntExpr) {
		error::OutputError(expr_line, ConditionJudgeInvalidType);
	}
	int relation_operator = token.get_token_sym();
	OperatorType operator_type = UNKNOWN_OPERATOR_TYPE;
	if (relation_operator != LSS && relation_operator != LEQ &&
		relation_operator != GRE && relation_operator != GEQ &&
		relation_operator != EQL && relation_operator != NEQ) {
		// 应为关系运算符
		// error();
	}
	else {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		expr_line = token.get_line();
		if (grammar::expr(roperand_ptr) != IntExpr) {
			error::OutputError(expr_line, ConditionJudgeInvalidType);
		}
		switch (relation_operator) {
		case LSS:
			operator_type = OperatorType::BGE;
			break;
		case LEQ:
			operator_type = OperatorType::BGT;
			break;
		case GRE:
			operator_type = OperatorType::BLE;
			break;
		case GEQ:
			operator_type = OperatorType::BLT;
			break;
		case EQL:
			operator_type = OperatorType::BNE;
			break;
		case NEQ:
			operator_type = OperatorType::BEQ;
			break;
		default:
			break;
		}
	}
	grammar::OutputGrammarElementName("<条件>");
	return operator_type;
}

// <有返回值函数调用语句>::=<标识符>'('<值参数表>')'
void grammar::func_call_with_ret() {
	if (token.get_token_sym() != IDENFR) {
		// error();
	}
	else {
		string func_name = token.get_token_str();
		if (!symbol_table::IsNameDefined(func_name)) {
			/*
			如果发现<未定义的名字>错误，首先报错，
			然后因为一行只有一个错误，后面一定有';'结束这句话，所以就一直读到';'，退出这个子程序
			*/
			error::OutputError(token.get_line(), ErrorType::NameNotDefine);
			grammar::OutputToken(token);
			token = grammar::GetToken();
			while (token.get_token_sym() != SEMICN) {
				grammar::OutputToken(token);
				token = grammar::GetToken();
			}
			grammar::OutputGrammarElementName("<有返回值函数调用语句>");
			return;
		}
		grammar::OutputToken(token);
		token = grammar::GetToken();
		ExpectedOrError(LPARENT);
		vector<ExprType> value_para_type_syms = grammar::value_para_tlb();
		// 此时token已经读到右小括号了，但是不存在恶意换行，所以可以直接用token.get_line()
		CheckParaRlt check_para_rst = symbol_table::CheckPara(func_name, value_para_type_syms);
		if (check_para_rst == CheckParaRlt::ParaNumNotMatch) {
			error::OutputError(token.get_line(), ErrorType::ArgsNumNotMatch);
		}
		else if (check_para_rst == CheckParaRlt::ParaTypeNotMatch) {
			error::OutputError(token.get_line(), ErrorType::ArgsTypeNotMatch);
		}
		ExpectedOrError(RPARENT);
		Operand* func_label_operand_ptr = new LabelOperand("_" + func_name + "_Label");
		ir::AddQuaternionBack(Quaternion(OperatorType::CALL, func_label_operand_ptr));
	}
	grammar::OutputGrammarElementName("<有返回值函数调用语句>");
}

// <无返回值函数调用语句>::=<标识符>'('<值参数表>')'
void grammar::func_call_no_ret() {
	if (token.get_token_sym() != IDENFR) {
		// error();
	}
	else {
		string func_name = token.get_token_str();
		if (!symbol_table::IsNameDefined(func_name)) {
			/*
			如果发现<未定义的名字>错误，首先报错，
			然后因为一行只有一个错误，后面一定有';'结束这句话，所以就一直读到';'，退出这个子程序
			*/
			error::OutputError(token.get_line(), ErrorType::NameNotDefine);
			grammar::OutputToken(token);
			token = grammar::GetToken();
			while (token.get_token_sym() != SEMICN) {
				grammar::OutputToken(token);
				token = grammar::GetToken();
			}
			grammar::OutputGrammarElementName("<无返回值函数调用语句>");
			return;
		}
		grammar::OutputToken(token);
		token = grammar::GetToken();
		ExpectedOrError(LPARENT);
		vector<ExprType> value_para_type_syms = grammar::value_para_tlb();
		// 此时token已经读到右小括号了，但是不存在恶意换行，所以可以直接用token.get_line()
		CheckParaRlt check_para_rlt = symbol_table::CheckPara(func_name, value_para_type_syms);
		if (check_para_rlt == CheckParaRlt::ParaNumNotMatch) {
			error::OutputError(token.get_line(), ErrorType::ArgsNumNotMatch);
		}
		else if (check_para_rlt == CheckParaRlt::ParaTypeNotMatch) {
			error::OutputError(token.get_line(), ErrorType::ArgsTypeNotMatch);
		}
		ExpectedOrError(RPARENT);
		Operand* func_label_operand_ptr = new LabelOperand("_" + func_name + "_Label");
		ir::AddQuaternionBack(Quaternion(OperatorType::CALL, func_label_operand_ptr));
	}
	grammar::OutputGrammarElementName("<无返回值函数调用语句>");
}

// <值参数表>::=<表达式>{,<表达式>} | <空>
vector<ExprType> grammar::value_para_tlb() {
	vector<ExprType> value_para_type_syms;
	vector<Operand*> value_para_values;
	if (grammar::is_expr()) {
		Operand* expr_operand_ptr = nullptr;
		value_para_type_syms.push_back(grammar::expr(expr_operand_ptr));
		value_para_values.push_back(expr_operand_ptr);
		while (token.get_token_sym() == COMMA) {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			value_para_type_syms.push_back(grammar::expr(expr_operand_ptr));
			value_para_values.push_back(expr_operand_ptr);
		}
		for (int i = 0; i < value_para_values.size(); i++) {
			ir::AddQuaternionBack(Quaternion(OperatorType::MV, new SymbolOperand("$a" + to_string(i)), value_para_values[i]));
		}
	}
	grammar::OutputGrammarElementName("<值参数表>");
	return value_para_type_syms;
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
		string symbol_name = token.get_token_str();
		int symbol_line = token.get_line();
		if (!symbol_table::IsNameDefined(symbol_name)) {
			error::OutputError(symbol_line, NameNotDefine);
		}
		if (symbol_table::IsConSymbol(symbol_table::GetSymbolLatestType(symbol_name))) {
			error::OutputError(symbol_line, ModifyConst);
		}
		Operand* symbol_operand_ptr = ir::GetSymbolOperandPtr(symbol_table::GetSymbolLatest(symbol_name));
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() == ASSIGN) {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			Operand* expr_operand_ptr = nullptr;
			grammar::expr(expr_operand_ptr);
			ir::AddQuaternionBack(Quaternion(MV, symbol_operand_ptr, expr_operand_ptr));
		}
		else if (token.get_token_sym() == LBRACK) {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			shared_ptr<ArrSymbol> arr_symbol_ptr = dynamic_pointer_cast<ArrSymbol>(symbol_table::GetSymbolLatest(symbol_name));
			//int arr_symbol_dim_1 = arr_symbol_ptr->get_arr_symbol_dim_1();
			Operand* dim_1_operand_ptr = nullptr;
			if (grammar::expr(dim_1_operand_ptr) != IntExpr) {
				error::OutputError(token.get_line(), ArrIndexNotInt);
			}
			ExpectedOrError(RBRACK);
			if (token.get_token_sym() == ASSIGN) {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				Operand* expr_operand_ptr = nullptr;
				grammar::expr(expr_operand_ptr);
				ir::AddQuaternionBack(Quaternion(OperatorType::STORE, expr_operand_ptr, symbol_operand_ptr, dim_1_operand_ptr));
			}
			else if (token.get_token_sym() == LBRACK) {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				int arr_symbol_dim_2 = arr_symbol_ptr->get_arr_symbol_dim_2();
				Operand* dim_2_operand_ptr = nullptr;
				if (grammar::expr(dim_2_operand_ptr) != IntExpr) {
					error::OutputError(token.get_line(), ArrIndexNotInt);
				}
				ExpectedOrError(RBRACK);
				ExpectedOrError(ASSIGN);
				Operand* expr_operand_ptr = nullptr;
				grammar::expr(expr_operand_ptr);
				// a[0..i..n][0..j..m]
				Operand* temp_operand_1_ptr = ir::ProdTempOperandPtr();
				Operand* dim_2_imme_operand_ptr = new ImmeOperand(arr_symbol_dim_2);
				// li $t0, m
				ir::AddQuaternionBack(Quaternion(OperatorType::LOAD_IMM, temp_operand_1_ptr, dim_2_imme_operand_ptr));
				Operand* temp_operand_2_ptr = ir::ProdTempOperandPtr();
				// mul $t1, i, $t0
				ir::AddQuaternionBack(Quaternion(OperatorType::MUL, temp_operand_2_ptr, dim_1_operand_ptr, temp_operand_1_ptr));
				temp_operand_1_ptr = ir::ProdTempOperandPtr();
				// add $t2, $t1, j
				ir::AddQuaternionBack(Quaternion(OperatorType::ADDU, temp_operand_1_ptr, temp_operand_2_ptr, dim_2_operand_ptr));
				ir::AddQuaternionBack(Quaternion(OperatorType::STORE, expr_operand_ptr, symbol_operand_ptr, temp_operand_1_ptr));
			}
			else {
				// error();
			}
		}
		else {
			// error();
		}
	}
	grammar::OutputGrammarElementName("<赋值语句>");
}

// <读语句>::=scanf'('<标识符>')'
void grammar::input_statement() {
	ExpectedOrError(SCANFTK);
	ExpectedOrError(LPARENT);
	if (token.get_token_sym() != IDENFR) {
		// error();
	}
	else {
		string symbol_name = token.get_token_str();
		int symbol_line = token.get_line();
		if (!symbol_table::IsNameDefined(symbol_name)) {
			error::OutputError(symbol_line, NameNotDefine);
		}
		SymbolType symbol_type = symbol_table::GetSymbolLatestType(symbol_name);
		if (symbol_table::IsConSymbol(symbol_type)) {
			error::OutputError(symbol_line, ModifyConst);
		}
		// 添加四元式
		shared_ptr<Symbol> symbol_ptr = symbol_table::GetSymbolLatest(symbol_name);
		if (symbol_type == IntVarSym) {
			ir::AddQuaternionBack(Quaternion(OperatorType::READ_INT, ir::GetSymbolOperandPtr(symbol_ptr)));
		}
		else if (symbol_type == CharVarSym) {
			ir::AddQuaternionBack(Quaternion(OperatorType::READ_CHAR, ir::GetSymbolOperandPtr(symbol_ptr)));
		}
		grammar::OutputToken(token);
		token = grammar::GetToken();
		ExpectedOrError(RPARENT);
	}
	grammar::OutputGrammarElementName("<读语句>");
}

// <写语句>::=printf'('<字符串>,<表达式>')' | printf'('<字符串>')' | printf'('<表达式>')'
void grammar::output_statement() {
	ExpectedOrError(PRINTFTK);
	ExpectedOrError(LPARENT);
	Operand* expr_operand_ptr = nullptr;
	if (token.get_token_sym() == STRCON) {
		string string_value = grammar::string_sym();
		// 生成StringOperand类的操作数
		Operand* string_operand_ptr = new StringOperand(ir::ProdStringName(), string_value);
		// 添加到quat_vector中
		// str_name: .asciiz "..."
		ir::AddQuaternionHead(Quaternion(OperatorType::STR_VAR, string_operand_ptr));
		ir::AddQuaternionBack(Quaternion(OperatorType::PRINT_STRING, string_operand_ptr));
		if (token.get_token_sym() == COMMA) {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			ExprType expr_type = grammar::expr(expr_operand_ptr);
			if (expr_type == IntExpr || expr_type == CharExpr) {
				OperatorType operator_type = expr_type == IntExpr ? OperatorType::PRINT_INT : OperatorType::PRINT_CHAR;
				ir::AddQuaternionBack(Quaternion(operator_type, expr_operand_ptr));
			}
			ExpectedOrError(RPARENT);
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
		ExprType expr_type = grammar::expr(expr_operand_ptr);
		if (expr_type == IntExpr || expr_type == CharExpr) {
			OperatorType operator_type = expr_type == IntExpr ? OperatorType::PRINT_INT : OperatorType::PRINT_CHAR;
			ir::AddQuaternionBack(Quaternion(operator_type, expr_operand_ptr));
		}
		ExpectedOrError(RPARENT);
	}
	// enter_str:	.asciiz	"\n";
	ir::AddQuaternionBack(Quaternion(OperatorType::PRINT_STRING, (Operand*)&enter_string_opreand_ptr));
	grammar::OutputGrammarElementName("<写语句>");
}

// <情况语句>::=switch '('<表达式>')' '{'<情况表><缺省>'}'
void grammar::case_statement() {
	ExpectedOrError(SWITCHTK);
	ExpectedOrError(LPARENT);
	Operand* expr_operand_ptr = nullptr;
	ExprType expr_type = grammar::expr(expr_operand_ptr);
	ExpectedOrError(RPARENT);
	ExpectedOrError(LBRACE);
	Operand* exit_label_operand_ptr = new LabelOperand(ProdLabelName());
	Operand* next_label_operand_ptr = nullptr;
	grammar::case_tlb(expr_type, expr_operand_ptr, next_label_operand_ptr, exit_label_operand_ptr);
	grammar::default_statement(next_label_operand_ptr);
	ExpectedOrError(RBRACE);
	ir::AddQuaternionBack(Quaternion(LABEL, exit_label_operand_ptr));
	grammar::OutputGrammarElementName("<情况语句>");
}

// <情况表>::=<情况子语句>{<情况子语句>}
void grammar::case_tlb(ExprType expr_type, Operand*& expr_operand_ptr, Operand*& next_label_operand_ptr, Operand*& exit_label_operand_ptr) {
	Operand* entry_label_operand_ptr = new LabelOperand(ProdLabelName());
	next_label_operand_ptr = new LabelOperand(ProdLabelName());
	grammar::case_substatement(expr_type, expr_operand_ptr, entry_label_operand_ptr, next_label_operand_ptr, exit_label_operand_ptr);
	while (token.get_token_sym() == CASETK) {
		entry_label_operand_ptr = next_label_operand_ptr;
		next_label_operand_ptr = new LabelOperand(ProdLabelName());
		grammar::case_substatement(expr_type, expr_operand_ptr, entry_label_operand_ptr, next_label_operand_ptr, exit_label_operand_ptr);
	}
	grammar::OutputGrammarElementName("<情况表>");
}

// <情况子语句>::=case <常量>: <语句>
void grammar::case_substatement(ExprType expr_type, Operand*& expr_operand_ptr, Operand* entry_label_operand_ptr, 
	Operand* next_label_operand_ptr, Operand*& exit_label_operand_ptr) {
	// Label_1:
	ir::AddQuaternionBack(Quaternion(OperatorType::LABEL, entry_label_operand_ptr));
	ExpectedOrError(CASETK);
	int instant_value = grammar::const_sym(expr_type);
	Operand* imme_operand_ptr = new ImmeOperand(instant_value);
	// BNE Label_2, <表达式>, <常量>
	ir::AddQuaternionBack(Quaternion(OperatorType::BNE, next_label_operand_ptr, expr_operand_ptr, imme_operand_ptr));
	
	ExpectedOrError(COLON);
	grammar::statement();
	// GOTO exit_label
	ir::AddQuaternionBack(Quaternion(OperatorType::GOTO, exit_label_operand_ptr));
	grammar::OutputGrammarElementName("<情况子语句>");
}

// <缺省>::=default: <语句>
void grammar::default_statement(Operand*& next_label_operand_ptr) {
	if (token.get_token_sym() != DEFAULTTK) {
		// 如果缺少缺省语句，报错后直接退出，因为后面一定是'}'
		error::OutputError(token.get_line(), DefaultMiss);
	}
	else {
		ir::AddQuaternionBack(Quaternion(LABEL, next_label_operand_ptr));
		grammar::OutputToken(token);
		token = grammar::GetToken();
		ExpectedOrError(COLON);
		grammar::statement();
	}
	grammar::OutputGrammarElementName("<缺省>");
}

// <返回语句>::=return['('<表达式>')']
void grammar::ret_statement() {
	ExpectedOrError(RETURNTK);
	if (return_state == ReturnState::InIntRetFuncNoReturn) {
		return_state = ReturnState::InIntRetFuncWithReturn;
	}
	else if (return_state == ReturnState::InCharRetFuncNoReturn) {
		return_state = ReturnState::InCharRetFuncWithReturn;
	}
	if (token.get_token_sym() == LPARENT) {
		if (return_state == ReturnState::InNoRetFunc) {
			// 无返回值的函数有形如return(表达式);或return();的语句
			error::OutputError(token.get_line(), ErrorType::NoRetFuncReturn);
			grammar::OutputToken(token);
			token = grammar::GetToken();
			while (token.get_token_sym() != SEMICN) {
				grammar::OutputToken(token);
				token = grammar::GetToken();
			}
		} 
		else {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			if (token.get_token_sym() == RPARENT && return_state >= 1) {
				// 有返回值的函数有形如return();的语句
				error::OutputError(token.get_line(), ErrorType::RetFuncMissReturn);
			} 
			else {
				Operand* expr_operand_ptr = nullptr;
				ExprType expr_type = grammar::expr(expr_operand_ptr);
				if ((return_state == ReturnState::InIntRetFuncWithReturn && expr_type != ExprType::IntExpr) ||
					(return_state == ReturnState::InCharRetFuncWithReturn && expr_type != ExprType::CharExpr)) {
					// 有返回值的函数的return语句中表达式类型与返回值类型不一致
					error::OutputError(token.get_line(), ErrorType::RetFuncMissReturn);
				}
				else {
					ir::AddQuaternionBack(Quaternion(OperatorType::RET, (Operand*)&v0_symbol_operand_ptr, expr_operand_ptr));
				}
			}
			ExpectedOrError(RPARENT);
		}
	}
	else if (token.get_token_sym() == SEMICN) {
		if ((return_state == ReturnState::InIntRetFuncNoReturn ||
			return_state == ReturnState::InCharRetFuncNoReturn || return_state == ReturnState::InIntRetFuncWithReturn ||
			return_state == ReturnState::InCharRetFuncWithReturn)) {
			// 有返回值的函数有形如return;的语句
			error::OutputError(token.get_line(), ErrorType::RetFuncMissReturn);
		} else {
			ir::AddQuaternionBack(Quaternion(OperatorType::RET, nullptr));
		}
	}
	grammar::OutputGrammarElementName("<返回语句>");
}

// <表达式>::=[+|-]<项>{<加法运算符><项>}
ExprType grammar::expr(Operand*& operand_ptr) {
	ExprType expr_type = VoidExpr;
	bool is_first_sym_minu = false;
	if (token.get_token_sym() == PLUS || token.get_token_sym() == MINU) {
		is_first_sym_minu = token.get_token_sym() == MINU ? true : false;
		expr_type = IntExpr;
		grammar::OutputToken(token);
		token = grammar::GetToken();
	}
	Operand* first_operand_ptr = nullptr;
	ExprType term_type = grammar::term(first_operand_ptr);
	expr_type = expr_type == VoidExpr ? term_type : expr_type;
	if (is_first_sym_minu) {
		Operand* temp_symbol_operand_ptr = ir::ProdTempOperandPtr();
		ir::AddQuaternionBack(Quaternion(OperatorType::SUBU, temp_symbol_operand_ptr, (Operand*)&zero_imme_operand_ptr, first_operand_ptr));
		first_operand_ptr = temp_symbol_operand_ptr;
	}
	Operand* next_operand_ptr = nullptr;
	while (token.get_token_sym() == PLUS || token.get_token_sym() == MINU) {
		OperatorType operator_type = token.get_token_sym() == PLUS ? OperatorType::ADDU : OperatorType::SUBU;
		expr_type = IntExpr;
		grammar::OutputToken(token);
		token = grammar::GetToken();
		// 已经确认是整型表达式
		grammar::term(next_operand_ptr);
		Operand* temp_symbol_operand_ptr = ir::ProdTempOperandPtr();
		ir::AddQuaternionBack(Quaternion(operator_type, temp_symbol_operand_ptr, first_operand_ptr, next_operand_ptr));
		first_operand_ptr = temp_symbol_operand_ptr;
	}
	operand_ptr = first_operand_ptr;
	grammar::OutputGrammarElementName("<表达式>");
	return expr_type;
}

// <项>::=<因子>{<乘法运算符><因子>}
ExprType grammar::term(Operand *&operand_ptr) {
	Operand *first_operand_ptr = nullptr, *next_operand_ptr = nullptr;
	ExprType term_type = grammar::factor(first_operand_ptr);
	while (token.get_token_sym() == MULT || token.get_token_sym() == DIV) {
		OperatorType operator_type = token.get_token_sym() == MULT ? OperatorType::MUL : OperatorType::DIVI;
		term_type = IntExpr;
		grammar::OutputToken(token);
		token = grammar::GetToken();
		grammar::factor(next_operand_ptr);
		Operand* temp_symbol_operand_ptr = ir::ProdTempOperandPtr();
		ir::AddQuaternionBack(Quaternion(operator_type, temp_symbol_operand_ptr, first_operand_ptr, next_operand_ptr));
		first_operand_ptr = temp_symbol_operand_ptr;
	}
	operand_ptr = first_operand_ptr;
	grammar::OutputGrammarElementName("<项>");
	return term_type;
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
ExprType grammar::factor(Operand *&operand_ptr) {
	ExprType factor_type = IntExpr;
	if (grammar::is_func_call()) {
		/*
		因为不会出现无返回值调用语句，
		只要出现标识符加'('的情况就进入有返回值函数调用语句,
		如果发现这个标识符没有被定义，就在<有返回值函数调用语句>中报错，并且直接跳出跳读至';'；
		*/
		string func_name = token.get_token_str();
		if (symbol_table::IsNameDefined(func_name) &&
			symbol_table::GetSymbolLatestType(func_name) == CharFuncSym) {
			factor_type = CharExpr;
		}
		grammar::func_call_with_ret();
		operand_ptr = ir::ProdTempOperandPtr();
		ir::AddQuaternionBack(Quaternion(MV, operand_ptr, (Operand*)&v0_symbol_operand_ptr));
	}
	else if (token.get_token_sym() == IDENFR) {
		string idenfr_name = token.get_token_str();
		if (!symbol_table::IsNameDefined(idenfr_name)) {
			error::OutputError(token.get_line(), NameNotDefine);
		}
		shared_ptr<Symbol> symbol_ptr = symbol_table::GetSymbolLatest(idenfr_name);
		Operand* idenfr_operand_ptr = ir::GetSymbolOperandPtr(symbol_ptr);
		SymbolType symbol_type = symbol_ptr->get_symbol_type();
		if (symbol_type == SymbolType::CharVarSym || symbol_type == SymbolType::CharConSym || 
			symbol_type == SymbolType::CharArrSym) {
			factor_type = ExprType::CharExpr;
		}
		grammar::OutputToken(token);
		token = grammar::GetToken();
		if (token.get_token_sym() == LBRACK) {
			grammar::OutputToken(token);
			token = grammar::GetToken();
			Operand* dim_1_operand_ptr = nullptr;
			if (grammar::expr(dim_1_operand_ptr) != ExprType::IntExpr) {
				error::OutputError(token.get_line(), ArrIndexNotInt);
			}
			ExpectedOrError(RBRACK);
			operand_ptr = ir::ProdTempOperandPtr();
			if (token.get_token_sym() == LBRACK) {
				grammar::OutputToken(token);
				token = grammar::GetToken();
				Operand* dim_2_operand_ptr = nullptr;
				if (grammar::expr(dim_2_operand_ptr) != ExprType::IntExpr) {
					error::OutputError(token.get_line(), ArrIndexNotInt);
				}
				ExpectedOrError(RBRACK);
				shared_ptr<ArrSymbol> arr_symbol_ptr = dynamic_pointer_cast<ArrSymbol>(symbol_table::GetSymbolLatest(idenfr_name));
				int arr_symbol_dim_2 = arr_symbol_ptr->get_arr_symbol_dim_2();
				// a[0..i..n][0..j..m]
				Operand* temp_operand_1_ptr = ir::ProdTempOperandPtr();
				Operand* dim_2_imme_operand_ptr = new ImmeOperand(arr_symbol_dim_2);
				// li $t0, m
				ir::AddQuaternionBack(Quaternion(OperatorType::LOAD_IMM, temp_operand_1_ptr, dim_2_imme_operand_ptr));
				Operand* temp_operand_2_ptr = ir::ProdTempOperandPtr();
				// mul $t1, i, $t0
				ir::AddQuaternionBack(Quaternion(OperatorType::MUL, temp_operand_2_ptr, dim_1_operand_ptr, temp_operand_1_ptr));
				temp_operand_1_ptr = ir::ProdTempOperandPtr();
				// add $t2, $t1, j
				ir::AddQuaternionBack(Quaternion(OperatorType::ADDU, temp_operand_1_ptr, temp_operand_2_ptr, dim_2_operand_ptr));
				ir::AddQuaternionBack(Quaternion(OperatorType::LOAD, operand_ptr, idenfr_operand_ptr, temp_operand_1_ptr));
			}
			else {
				ir::AddQuaternionBack(Quaternion(OperatorType::LOAD, operand_ptr, idenfr_operand_ptr, dim_1_operand_ptr));
			}
		}
		else {
			// <标识符>
			if (symbol_table::IsConSymbol(symbol_ptr->get_symbol_type())) {
				shared_ptr<VarSymbol> var_symbol_ptr = dynamic_pointer_cast<VarSymbol>(symbol_ptr);
				operand_ptr = new ImmeOperand(var_symbol_ptr->get_var_symbol_value());
			}
			else {
				operand_ptr = idenfr_operand_ptr;
			}
		}
	}
	else if (token.get_token_sym() == LPARENT) {
		grammar::OutputToken(token);
		token = grammar::GetToken();
		grammar::expr(operand_ptr);
		ExpectedOrError(RPARENT);
	}
	else if (token.get_token_sym() == PLUS || token.get_token_sym() == MINU || token.get_token_sym() == INTCON) {
		int value = grammar::integer();
		operand_ptr = new ImmeOperand(value);
	}
	else if (token.get_token_sym() == CHARCON) {
		factor_type = CharExpr;
		int value = token.get_token_str()[0];
		operand_ptr = new ImmeOperand(value);
		grammar::OutputToken(token);
		token = grammar::GetToken();
	}
	else {
		// error();
	}
	grammar::OutputGrammarElementName("<因子>");
	return factor_type;
}

// <字符串>::="{十进制编码为32,33,35-126的ASCII字符}"
string grammar::string_sym() {
	string string_value = "";
	if (token.get_token_sym() != STRCON) {
		// error();
	}
	else {
		string_value = token.get_token_str();
		grammar::OutputToken(token);
		token = grammar::GetToken();
	}
	grammar::OutputGrammarElementName("<字符串>");
	return string_value;
}

// <常量>::=<整数> | <字符>
int grammar::const_sym(ExprType expr_type) {
	if ((expr_type == ExprType::CharExpr && token.get_token_sym() != CHARCON) ||
		(expr_type == ExprType::IntExpr && token.get_token_sym() == CHARCON)) {
		error::OutputError(token.get_line(), VarTypeNotMatch);
	}
	int value;
	if (token.get_token_sym() == CHARCON) {
		value = token.get_token_str()[0];
		grammar::OutputToken(token);
		token = grammar::GetToken();
	}
	else {
		value = grammar::integer();
	}
	grammar::OutputGrammarElementName("<常量>");
	return value;
}

// <整数>::=[+|-]<无符号整数>
int grammar::integer() {
	bool is_minu = false;
	if (token.get_token_sym() == PLUS || token.get_token_sym() == MINU) {
		is_minu = token.get_token_sym() == MINU ? true : false;
		grammar::OutputToken(token);
		token = grammar::GetToken();
	}
	int num = grammar::unsigned_integer();
	grammar::OutputGrammarElementName("<整数>");
	return is_minu ? -num : num;
}

// <无符号整数>::= <数字>{<数字>}
int grammar::unsigned_integer() {
	int num = -1;
	if (token.get_token_sym() != INTCON) {
		// error();
	}
	else {
		num = stoi(token.get_token_str());
		grammar::OutputToken(token);
		token = grammar::GetToken();
	}
	grammar::OutputGrammarElementName("<无符号整数>");
	return num;
}