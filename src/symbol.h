#pragma once
#include <string>
#include <vector>
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
};

class Symbol {
protected:
	std::string symbol_name;
	SymbolType symbol_type;
	int symbol_layer;	// 符号声明所在的分程序的层次,最外层为0
public:
	Symbol(std::string, SymbolType, int);
	~Symbol(void) {};
	std::string get_symbol_name() { return symbol_name; }
	SymbolType get_symbol_type() { return symbol_type; }
	int get_symbol_layer() { return symbol_layer; }
};

class VarSymbol : public Symbol {
private:
	int var_symbol_value;
public:
	VarSymbol(std::string, SymbolType, int);
	~VarSymbol(void) {};
	void set_var_symbol_value(int value) { var_symbol_value = value; }
	int get_var_symbol_value() { return var_symbol_value; }
};

class ArrSymbol : public Symbol {
private:
	std::vector<int> arr_symbol_dimension;
public:
	ArrSymbol(std::string, SymbolType, int);
	~ArrSymbol(void) {};
	void add_arr_symbol_dimension(int dimension) { arr_symbol_dimension.push_back(dimension); }
	std::vector<int> get_arr_symbol_dimension() { return arr_symbol_dimension; }
};

class FuncSymbol : public Symbol {
private:
	std::vector<VarSymbol> func_symbol_args;
public:
	FuncSymbol(std::string, SymbolType, int);
	~FuncSymbol(void) {};
	void add_func_symbol_args(VarSymbol arg) { func_symbol_args.push_back(arg); }
	std::vector<VarSymbol> get_func_symbol_args() { return func_symbol_args; }
};

namespace symbol_table {
	std::vector<Symbol> symbol_vector;	// 符号list，按顺序记录符号
	/*
	name2index_map记录某符号名在symbol_vector中对应的索引，因为在不同的分程序中可以存在同名的符号。
	每次插入符号时，如果name2index_map存在相同name的键，
	只需要判断该name对应的索引vector的最后一项索引对应的符号的layer和要插入的符号的layer是否相同即可。
	*/
	std::unordered_map<std::string, std::vector<int>> name2index_map;

	
}