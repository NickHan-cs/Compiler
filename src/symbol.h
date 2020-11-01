#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// 符号类型
enum SymbolType {
	IntConSym,		// 整型常量符号
	CharConSym,		// 字符常量符号
	IntVarSym,		// 整型变量符号
	CharVarSym,		// 字符变量符号
	IntArrSym,		// 整型数组符号
	CharArrSym,		// 字符数组符号
	VoidFuncSym,	// 无返回值函数符号
	IntFuncSym,		// 返回整型函数符号
	CharFuncSym,	// 返回字符函数符号
	OtherTypeSym
};

enum ExprType {
	IntExpr,	// 整型表达式
	CharExpr,	// 字符型表达式
	VoidExpr
};

enum CheckParaRlt {
	NoFault,			// 没有错误
	ParaNumNotMatch,	// 参数个数不匹配
	ParaTypeNotMatch,	// 参数类型不匹配
	OtherFault			// 调用的标识符不是函数
};

class Symbol {
protected:
	std::string symbol_name;
	SymbolType symbol_type;
	int symbol_level;	// 符号声明所在的分程序的层次,最外层为0
public:
	Symbol(std::string, SymbolType, int);
	virtual ~Symbol(void) {};
	std::string get_symbol_name() { return symbol_name; }
	SymbolType get_symbol_type() { return symbol_type; }
	int get_symbol_level() { return symbol_level; }
};

class VarSymbol : public Symbol {
private:
	int var_symbol_value;
public:
	VarSymbol(std::string name, SymbolType type, int level);
	~VarSymbol(void) {};
	void set_var_symbol_value(int value) { var_symbol_value = value; }
	int get_var_symbol_value() { return var_symbol_value; }
};

class ArrSymbol : public Symbol {
private:
	int arr_symbol_dim_1;
	int arr_symbol_dim_2;
public:
	ArrSymbol(std::string name, SymbolType type, int level, int arr_symbol_dim_1, int arr_symbol_dim_2);
	~ArrSymbol(void) {};
	int get_arr_symbol_dim_1() { return arr_symbol_dim_1; }
	int get_arr_symbol_dim_2() { return arr_symbol_dim_2; }
};

class FuncSymbol : public Symbol {
private:
	std::vector<std::shared_ptr<VarSymbol>> func_symbol_args_ptrs;
public:
	FuncSymbol(std::string name, SymbolType type, int level);
	~FuncSymbol(void) {};
	void add_func_symbol_args_ptr(std::shared_ptr<VarSymbol> arg_ptr) { func_symbol_args_ptrs.push_back(arg_ptr); }
	std::vector<std::shared_ptr<VarSymbol>> get_func_symbol_args_ptrs() { return func_symbol_args_ptrs; }
};

namespace symbol_table {

	// 检查名字是否被定义，如果是，返回true；否则，返回false
	bool IsNameDefined(std::string symbol_name);

	// 检查名字是否重定义。如果是，返回true；否则，返回false。
	bool IsNameRedefined(std::string symbol_name, int symbol_level);
	
	/*
	将新的symbol添加到name2index_map中。
	如果name2index_map中已经包含这个符号的symbol_name，就直接添加到对应的索引向量中；
	否则，先添加该symbol_name对应的键值对，再添加该symbol。
	*/
	void AddNewSymbol2Map(std::string symbol_name);

	// 向符号表中添加常量符号和变量符号
	bool AddVarSymbol(std::string var_name, SymbolType symbol_type, int symbol_level);
	
	// 向符号表中添加数组符号
	bool AddArrSymbol(std::string arr_name, SymbolType symbol_type, int symbol_level, int arr_symbol_dim_1, int arr_symbol_dim_2);

	// 向符号表中添加函数符号
	bool AddFuncSymbol(std::string func_name, SymbolType symbol_type, int symbol_level);

	// 删除该level的标识符
	void DelCurLevelSymbol(int cur_level);

	bool IsConSymbol(SymbolType symbol_type);

	bool IsVarSymbol(SymbolType symbol_type);

	bool IsArrSymbol(SymbolType symbol_type);

	bool IsFuncSymbol(SymbolType symbol_type);

	/*
	判断func_name对应的标识符类型是否是IntFuncSym或CharFuncSym（调用GetSymbolLatestType即可）。
	如果是，返回true；否则，返回false。
	*/
	bool IsWithRetFunc(std::string func_name);

	/*
	得到最后一次插入的名为symbol_name的标识符的symbol_type。
	如果name2index_map中不存在symbol_name的键，返回OtherTypeSym。
	*/
	SymbolType GetSymbolLatestType(std::string symbol_name);

	/*
	调用函数时，检查值参数表和函数参数表是否匹配。
	如果匹配，返回0；
	如果函数参数个数不匹配，返回1；
	如果函数参数类型不匹配，返回2；
	如果是其他错误，返回3。
	*/
	CheckParaRlt CheckPara(std::string func_name, std::vector<ExprType> value_para_type_syms);

	std::shared_ptr<Symbol> GetSymbolPtrVectorBack();
}