#include <cassert>
#include <algorithm>
#include "symbol.h"

using namespace std;

// 符号栈，按顺序记录指向符号的指针
vector<shared_ptr<Symbol>> symbol_ptr_vector;

/*
name2index_map记录某符号名在symbol_ptr_vector中对应的索引。
由于在不同的作用域中可以存在同名的符号，所以每次插入新的符号时，
如果name2index_map中有相同符号名的键，就获取最后一次放入name2index_map的同名符号，
如果该符号的symbol_level和新符号的symbol_level相等，则说明发生<名字重定义>错误；
否则，将新符号插入到name2index_map中。
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
	// 理论上不需要下面两句代码，因为当DelCurLevelSymbol时，如果某个键对应的值为空时，整个键值对都被删除了
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
		// 如果在name2index_map中不存在这个symbol_name，那么需要先新建一个键值对
		shared_ptr<vector<int>> new_index_vector_ptr = make_shared<vector<int>>();
		name2index_map.insert(make_pair(symbol_name, new_index_vector_ptr));
	}
	/*
	因为vector是从0开始编号的且此时新符号已经添加到symbol_ptr_vector中，
	所以添加新符号的索引的时候要添加symbol_ptr_vector.size()-1
	*/
	name2index_map.at(symbol_name)->push_back(symbol_ptr_vector.size() - 1);
}

bool symbol_table::AddVarSymbol(string var_name, SymbolType symbol_type, int symbol_level) {
	// 符号表中不区分大小写，统一转小写
	transform(var_name.begin(), var_name.end(), var_name.begin(), ::tolower);
	if (symbol_table::IsNameRedefined(var_name, symbol_level)) {
		// 出现了<名字重定义>错误
		return false;
	}
	// 符号栈中加入新的变量符号或常量符号
	shared_ptr<Symbol> new_symbol_ptr = make_shared<VarSymbol>(var_name, symbol_type, symbol_level);
	symbol_ptr_vector.push_back(new_symbol_ptr);
	// name2index_map中加入新的符号索引
	symbol_table::AddNewSymbol2Map(var_name);
	return true;
}

bool symbol_table::AddArrSymbol(string arr_name, SymbolType symbol_type, int symbol_level, int arr_symbol_dim_1, int arr_symbol_dim_2) {
	// 符号表中不区分大小写，统一转小写
	transform(arr_name.begin(), arr_name.end(), arr_name.begin(), ::tolower);
	if (symbol_table::IsNameRedefined(arr_name, symbol_level)) {
		// 出现了<名字重定义>错误
		return false;
	}
	// 符号栈中加入新的数组符号
	shared_ptr<Symbol> new_symbol_ptr = make_shared<ArrSymbol>(arr_name, symbol_type, symbol_level, arr_symbol_dim_1, arr_symbol_dim_2);
	symbol_ptr_vector.push_back(new_symbol_ptr);
	// name2index_map中加入新的符号索引
	symbol_table::AddNewSymbol2Map(arr_name);
	return true;
}

bool symbol_table::AddFuncSymbol(string func_name, SymbolType symbol_type, int symbol_level) {
	// 符号表中不区分大小写，统一转小写
	transform(func_name.begin(), func_name.end(), func_name.begin(), ::tolower);
	if (symbol_table::IsNameRedefined(func_name, symbol_level)) {
		// 出现了<名字重定义>错误
		return false;
	}
	// 符号栈中加入新的函数符号
	shared_ptr<Symbol> new_symbol_ptr = make_shared<FuncSymbol>(func_name, symbol_type, symbol_level);
	symbol_ptr_vector.push_back(new_symbol_ptr);
	// name2index_map中加入新的符号索引
	symbol_table::AddNewSymbol2Map(func_name);
	return true;
}

void symbol_table::DelCurLevelSymbol(int cur_level) {
	// 这里是直接删除符号还是将其移动到另一片内存空间，有待商榷，暂且选择删除
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
			// 如果删除该符号后，发现该作用域外没有该符号，就从name2index_map中删除这个键值对
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
