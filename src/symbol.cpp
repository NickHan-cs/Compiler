#include <cassert>
#include <algorithm>
#include "symbol.h"

using namespace std;

// ����ջ����˳���¼ָ����ŵ�ָ��
vector<shared_ptr<Symbol>> symbol_ptr_vector;

/*
name2index_map��¼ĳ��������symbol_ptr_vector�ж�Ӧ��������
�����ڲ�ͬ���������п��Դ���ͬ���ķ��ţ�����ÿ�β����µķ���ʱ��
���name2index_map������ͬ�������ļ����ͻ�ȡ���һ�η���name2index_map��ͬ�����ţ�
����÷��ŵ�symbol_level���·��ŵ�symbol_level��ȣ���˵������<�����ض���>����
���򣬽��·��Ų��뵽name2index_map�С�
*/
unordered_map<string, shared_ptr<std::vector<int>>> name2index_map;


Symbol::Symbol(string name, SymbolType type, int level) {
	symbol_name = name;
	symbol_type = type;
	symbol_level = level;
}

VarSymbol::VarSymbol(string name, SymbolType type, int level): Symbol(name, type, level){
}

ArrSymbol::ArrSymbol(string name, SymbolType type, int level, int dim_1, int dim_2) : Symbol(name, type, level) {
	arr_symbol_dim_1 = dim_1;
	arr_symbol_dim_2 = dim_2;
}

FuncSymbol::FuncSymbol(string name, SymbolType type, int level) : Symbol(name, type, level) {
}

bool symbol_table::IsNameDefined(string symbol_name) {
	transform(symbol_name.begin(), symbol_name.end(), symbol_name.begin(), ::tolower);
	if (name2index_map.find(symbol_name) == name2index_map.end()) {
		return false;
	}
	// �����ϲ���Ҫ����������룬��Ϊ��DelCurLevelSymbolʱ�����ĳ������Ӧ��ֵΪ��ʱ��������ֵ�Զ���ɾ����
	shared_ptr<vector<int>> index_vector_ptr = name2index_map.at(symbol_name);
	return !index_vector_ptr->empty();
}

bool symbol_table::IsNameRedefined(string symbol_name, int symbol_level) {
	transform(symbol_name.begin(), symbol_name.end(), symbol_name.begin(), ::tolower);
	if (name2index_map.find(symbol_name) == name2index_map.end()) {
		return false;
	}
	shared_ptr<vector<int>> index_vector_ptr = name2index_map.at(symbol_name);
	return !index_vector_ptr->empty() &&
		symbol_ptr_vector.at(index_vector_ptr->back())->get_symbol_level() == symbol_level;
}

void symbol_table::AddNewSymbol2Map(string symbol_name) {
	if (name2index_map.find(symbol_name) == name2index_map.end()) {
		// �����name2index_map�в��������symbol_name����ô��Ҫ���½�һ����ֵ��
		shared_ptr<vector<int>> new_index_vector_ptr = make_shared<vector<int>>();
		name2index_map.insert(make_pair(symbol_name, new_index_vector_ptr));
	}
	/*
	��Ϊvector�Ǵ�0��ʼ��ŵ��Ҵ�ʱ�·����Ѿ���ӵ�symbol_ptr_vector�У�
	��������·��ŵ�������ʱ��Ҫ���symbol_ptr_vector.size()-1
	*/
	name2index_map.at(symbol_name)->push_back(symbol_ptr_vector.size() - 1);
}

bool symbol_table::AddVarSymbol(string var_name, SymbolType symbol_type, int symbol_level) {
	// ���ű��в����ִ�Сд��ͳһתСд
	transform(var_name.begin(), var_name.end(), var_name.begin(), ::tolower);
	if (symbol_table::IsNameRedefined(var_name, symbol_level)) {
		// ������<�����ض���>����
		return false;
	}
	// ����ջ�м����µı������Ż�������
	shared_ptr<Symbol> new_symbol_ptr = make_shared<VarSymbol>(var_name, symbol_type, symbol_level);
	symbol_ptr_vector.push_back(new_symbol_ptr);
	// name2index_map�м����µķ�������
	symbol_table::AddNewSymbol2Map(var_name);
	return true;
}

bool symbol_table::AddArrSymbol(string arr_name, SymbolType symbol_type, int symbol_level, int arr_symbol_dim_1, int arr_symbol_dim_2) {
	// ���ű��в����ִ�Сд��ͳһתСд
	transform(arr_name.begin(), arr_name.end(), arr_name.begin(), ::tolower);
	if (symbol_table::IsNameRedefined(arr_name, symbol_level)) {
		// ������<�����ض���>����
		return false;
	}
	// ����ջ�м����µ��������
	shared_ptr<Symbol> new_symbol_ptr = make_shared<ArrSymbol>(arr_name, symbol_type, symbol_level, arr_symbol_dim_1, arr_symbol_dim_2);
	symbol_ptr_vector.push_back(new_symbol_ptr);
	// name2index_map�м����µķ�������
	symbol_table::AddNewSymbol2Map(arr_name);
	return true;
}

bool symbol_table::AddFuncSymbol(string func_name, SymbolType symbol_type, int symbol_level) {
	// ���ű��в����ִ�Сд��ͳһתСд
	transform(func_name.begin(), func_name.end(), func_name.begin(), ::tolower);
	if (symbol_table::IsNameRedefined(func_name, symbol_level)) {
		// ������<�����ض���>����
		return false;
	}
	// ����ջ�м����µĺ�������
	shared_ptr<Symbol> new_symbol_ptr = make_shared<FuncSymbol>(func_name, symbol_type, symbol_level);
	symbol_ptr_vector.push_back(new_symbol_ptr);
	// name2index_map�м����µķ�������
	symbol_table::AddNewSymbol2Map(func_name);
	return true;
}

void symbol_table::DelCurLevelSymbol(int cur_level) {
	// ������ֱ��ɾ�����Ż��ǽ����ƶ�����һƬ�ڴ�ռ䣬�д���ȶ������ѡ��ɾ��
	if (symbol_ptr_vector.empty()) {
		return;
	}
	shared_ptr<Symbol> symbol_ptr = symbol_ptr_vector.back();
	while (symbol_ptr->get_symbol_level() == cur_level) {
		string symbol_name = symbol_ptr->get_symbol_name();

		assert(name2index_map.find(symbol_name) != name2index_map.end());
		shared_ptr<vector<int>> index_vector_ptr = name2index_map.at(symbol_name);
		
		assert(index_vector_ptr->back() == symbol_ptr_vector.size() - 1);
		index_vector_ptr->pop_back();
		if (index_vector_ptr->empty()) {
			// ���ɾ���÷��ź󣬷��ָ���������û�и÷��ţ��ʹ�name2index_map��ɾ�������ֵ��
			name2index_map.erase(symbol_name);
		}

		symbol_ptr_vector.pop_back();
		if (symbol_ptr_vector.empty()) {
			return;
		}
		symbol_ptr = symbol_ptr_vector.back();
	}
}

bool symbol_table::IsConSymbol(SymbolType symbol_type) {
	return symbol_type == IntConSym || symbol_type == CharConSym;
}

bool symbol_table::IsVarSymbol(SymbolType symbol_type) {
	return symbol_type == IntVarSym || symbol_type == CharVarSym;
}

bool symbol_table::IsArrSymbol(SymbolType symbol_type) {
	return symbol_type == IntArrSym || symbol_type == CharArrSym;
}

bool symbol_table::IsFuncSymbol(SymbolType symbol_type) {
	return symbol_type == VoidFuncSym || symbol_type == CharFuncSym || symbol_type == IntFuncSym;
}

bool symbol_table::IsWithRetFunc(string func_name) {
	transform(func_name.begin(), func_name.end(), func_name.begin(), ::tolower);
	return symbol_table::GetSymbolLatestType(func_name) == IntFuncSym ||
		symbol_table::GetSymbolLatestType(func_name) == CharFuncSym;
}

SymbolType symbol_table::GetSymbolLatestType(string symbol_name) {
	transform(symbol_name.begin(), symbol_name.end(), symbol_name.begin(), ::tolower);
	if (name2index_map.find(symbol_name) == name2index_map.end()) {
		return OtherTypeSym;
	}
	shared_ptr<vector<int>> index_vector_ptr = name2index_map.at(symbol_name);
	if (index_vector_ptr->empty()) {
		return OtherTypeSym;
	}
	return symbol_ptr_vector.at(index_vector_ptr->back())->get_symbol_type();
}

CheckParaRlt symbol_table::CheckPara(string func_name, vector<ExprType> value_para_type_syms) {
	transform(func_name.begin(), func_name.end(), func_name.begin(), ::tolower);
	if (!symbol_table::IsFuncSymbol(symbol_table::GetSymbolLatestType(func_name))) {
		return OtherFault;
	}
	shared_ptr<vector<int>> index_vector_ptr = name2index_map.at(func_name);
	shared_ptr<FuncSymbol> func_symbol_ptr =
		dynamic_pointer_cast<FuncSymbol>(symbol_ptr_vector.at(index_vector_ptr->back()));
	vector<shared_ptr<VarSymbol>> func_symbol_args = func_symbol_ptr->get_func_symbol_args_ptrs();
	if (func_symbol_args.size() != value_para_type_syms.size()) {
		return ParaNumNotMatch;
	}
	int args_size = func_symbol_args.size();
	for (int i = 0; i < args_size; i += 1) {
		if ((func_symbol_args.at(i)->get_symbol_type() == IntVarSym && value_para_type_syms.at(i) != IntExpr) ||
			(func_symbol_args.at(i)->get_symbol_type() == CharVarSym && value_para_type_syms.at(i) != CharExpr)) {
			return ParaTypeNotMatch;
		}
	}
	return NoFault;
}

shared_ptr<Symbol> symbol_table::GetSymbolPtrVectorBack() {
	return symbol_ptr_vector.back();
}
