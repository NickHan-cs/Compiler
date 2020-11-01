#pragma once
#include <string>
#include <vector>
#include <memory>
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
	OtherTypeSym
};

enum ExprType {
	IntExpr,	// ���ͱ��ʽ
	CharExpr,	// �ַ��ͱ��ʽ
	VoidExpr
};

enum CheckParaRlt {
	NoFault,			// û�д���
	ParaNumNotMatch,	// ����������ƥ��
	ParaTypeNotMatch,	// �������Ͳ�ƥ��
	OtherFault			// ���õı�ʶ�����Ǻ���
};

class Symbol {
protected:
	std::string symbol_name;
	SymbolType symbol_type;
	int symbol_level;	// �����������ڵķֳ���Ĳ��,�����Ϊ0
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

	// ��������Ƿ񱻶��壬����ǣ�����true�����򣬷���false
	bool IsNameDefined(std::string symbol_name);

	// ��������Ƿ��ض��塣����ǣ�����true�����򣬷���false��
	bool IsNameRedefined(std::string symbol_name, int symbol_level);
	
	/*
	���µ�symbol��ӵ�name2index_map�С�
	���name2index_map���Ѿ�����������ŵ�symbol_name����ֱ����ӵ���Ӧ�����������У�
	��������Ӹ�symbol_name��Ӧ�ļ�ֵ�ԣ�����Ӹ�symbol��
	*/
	void AddNewSymbol2Map(std::string symbol_name);

	// ����ű�����ӳ������źͱ�������
	bool AddVarSymbol(std::string var_name, SymbolType symbol_type, int symbol_level);
	
	// ����ű�������������
	bool AddArrSymbol(std::string arr_name, SymbolType symbol_type, int symbol_level, int arr_symbol_dim_1, int arr_symbol_dim_2);

	// ����ű�����Ӻ�������
	bool AddFuncSymbol(std::string func_name, SymbolType symbol_type, int symbol_level);

	// ɾ����level�ı�ʶ��
	void DelCurLevelSymbol(int cur_level);

	bool IsConSymbol(SymbolType symbol_type);

	bool IsVarSymbol(SymbolType symbol_type);

	bool IsArrSymbol(SymbolType symbol_type);

	bool IsFuncSymbol(SymbolType symbol_type);

	/*
	�ж�func_name��Ӧ�ı�ʶ�������Ƿ���IntFuncSym��CharFuncSym������GetSymbolLatestType���ɣ���
	����ǣ�����true�����򣬷���false��
	*/
	bool IsWithRetFunc(std::string func_name);

	/*
	�õ����һ�β������Ϊsymbol_name�ı�ʶ����symbol_type��
	���name2index_map�в�����symbol_name�ļ�������OtherTypeSym��
	*/
	SymbolType GetSymbolLatestType(std::string symbol_name);

	/*
	���ú���ʱ�����ֵ������ͺ����������Ƿ�ƥ�䡣
	���ƥ�䣬����0��
	�����������������ƥ�䣬����1��
	��������������Ͳ�ƥ�䣬����2��
	������������󣬷���3��
	*/
	CheckParaRlt CheckPara(std::string func_name, std::vector<ExprType> value_para_type_syms);

	std::shared_ptr<Symbol> GetSymbolPtrVectorBack();
}