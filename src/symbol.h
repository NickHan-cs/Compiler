#pragma once
#include <string>
#include <vector>
#include <unordered_map>

// ��������
enum SymbolType {
	IntConSym,		// ���ͳ�������
	CharConSym,		// �ַ���������
	IntVarSym,		// ���ͱ�������
	CharVarSym,		// �ַ���������
	IntArrSym,		// �����������
	CharArrSym,		// �ַ��������
	VoidFuncSym,	// �޷���ֵ��������
	IntFuncSym,		// �������ͺ�������
	CharFuncSym,	// �����ַ���������
};

class Symbol {
protected:
	std::string symbol_name;
	SymbolType symbol_type;
	int symbol_layer;	// �����������ڵķֳ���Ĳ��,�����Ϊ0
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
	std::vector<Symbol> symbol_vector;	// ����list����˳���¼����
	/*
	name2index_map��¼ĳ��������symbol_vector�ж�Ӧ����������Ϊ�ڲ�ͬ�ķֳ����п��Դ���ͬ���ķ��š�
	ÿ�β������ʱ�����name2index_map������ͬname�ļ���
	ֻ��Ҫ�жϸ�name��Ӧ������vector�����һ��������Ӧ�ķ��ŵ�layer��Ҫ����ķ��ŵ�layer�Ƿ���ͬ���ɡ�
	*/
	std::unordered_map<std::string, std::vector<int>> name2index_map;

	
}