#include <unordered_map>
#include <string>
#include <cassert>
#include <algorithm>
#include <iterator>
#include "basic_block.h"

using namespace std;

vector<BasicBlock*> DivideBasicBlocks(shared_ptr<vector<Quaternion>> ir_vector_ptr) {
	vector<BasicBlock*> basic_block_ptrs;
	int begin_ir_idx = 0;
	int start_i = 0;
	while ((*ir_vector_ptr)[start_i].get_op() != OperatorType::LABEL) {
		start_i++;
	}
	start_i += 1;
	for (int i = start_i; i < ir_vector_ptr->size(); i++) {
		OperatorType op = (*ir_vector_ptr)[i].get_op();
		// 当前指令属于前一个基本块
		if (op >= OperatorType::BLT && op <= OperatorType::LABEL) {
			basic_block_ptrs.push_back(new BasicBlock(begin_ir_idx, i));
			begin_ir_idx = i + 1;
		}
	}
	basic_block_ptrs.push_back(new BasicBlock(begin_ir_idx, ir_vector_ptr->size() - 1));
	GenFlowGraph(ir_vector_ptr, basic_block_ptrs);
	LiveVariableAnalysis(ir_vector_ptr, basic_block_ptrs);
	return basic_block_ptrs;
}

void GenFlowGraph(shared_ptr<vector<Quaternion>> const ir_vector_ptr, const vector<BasicBlock*> basic_block_ptrs) {
	// key-中间代码的位置	value-该中间代码所处的基本块指针
	unordered_map<int, BasicBlock*> ir_idx2basic_block_ptr;
	// key-标签名	value-该标签在中间代码中的位置
	unordered_map<string, int> label_name2ir_idx;
	for (auto basic_block_ptr : basic_block_ptrs) {
		for (int i = basic_block_ptr->begin_ir_idx; i <= basic_block_ptr->end_ir_idx; i++) {
			ir_idx2basic_block_ptr[i] = basic_block_ptr;
		}
	}
	for (int i = 0; i < ir_vector_ptr->size(); i++) {
		OperatorType op = (*ir_vector_ptr)[i].get_op();
		if (op == OperatorType::LABEL) {
			label_name2ir_idx[(*ir_vector_ptr)[i].get_dest_ptr()->ToString()] = i;
		}
	}
	for (auto basic_block_ptr : basic_block_ptrs) {
		// B1的最后一条语句有条件或无条件跳转到B2
		// B1最后的条件转移指令的判断条件失败后进入到B2执行
		// B1中出现了函数调用，跳转到另一个函数
		for (int i = basic_block_ptr->begin_ir_idx; i <= basic_block_ptr->end_ir_idx; i++) {
			OperatorType op = (*ir_vector_ptr)[i].get_op();
			if ((op >= OperatorType::BLT && op <= OperatorType::GOTO) || op == OperatorType::CALL) {
				// B2的label_name
				string label_name = (*ir_vector_ptr)[i].get_dest_ptr()->ToString();
				assert(label_name2ir_idx.find(label_name) != label_name2ir_idx.end());
				// B2的label在中间代码中的位置
				int ir_idx = label_name2ir_idx[label_name];
				// if防止已读到整个程序的末尾
				// +1是因为Label中间代码属于前一个基本块，它的下一句才是B2的开始
				if (ir_idx2basic_block_ptr.find(ir_idx + 1) != ir_idx2basic_block_ptr.end()) {
					basic_block_ptr->AddNextBasicBlock(ir_idx2basic_block_ptr[ir_idx + 1]);
				}
			}
		}
		// B2的第一句语句紧跟在B1的最后一条语句之后，且B1的最后一条语句不为无条件跳转指令
		int block_end_ir_idx = basic_block_ptr->end_ir_idx;
		if ((*ir_vector_ptr)[block_end_ir_idx].get_op() != OperatorType::GOTO && 
			ir_idx2basic_block_ptr.find(block_end_ir_idx + 1) != ir_idx2basic_block_ptr.end()) {
				basic_block_ptr->AddNextBasicBlock(ir_idx2basic_block_ptr[block_end_ir_idx + 1]);
		}
	}
}

void GenUseDefSet(shared_ptr<vector<Quaternion>> const ir_vector_ptr, const vector<BasicBlock*> basic_block_ptrs) {
	for (auto basic_block_ptr : basic_block_ptrs) {
		// 记录在该基本块中已经出现过标识符操作数名
		set<string> visited_var_symbol_names;
		for (int i = basic_block_ptr->begin_ir_idx; i <= basic_block_ptr->end_ir_idx; i++) {
			OperatorType op = (*ir_vector_ptr)[i].get_op();
			// 如果是变量定义语句就跳过
			if (op >= OperatorType::VAR && op <= OperatorType::PARAM) {
				continue;
			}
			// 因为类似x = x + y的语句中x属于use集合,所以要先处理左操作数和右操作数
			Operand* loperand_ptr = (*ir_vector_ptr)[i].get_loperand_ptr();
			if (loperand_ptr != nullptr && ir::IsVarSymbolOperand(loperand_ptr) &&
				visited_var_symbol_names.find(loperand_ptr->ToString()) == visited_var_symbol_names.end()) {
				string var_symbol_name = loperand_ptr->ToString();
				visited_var_symbol_names.insert(var_symbol_name);
				basic_block_ptr->use_set.insert(var_symbol_name);
			}

			Operand* roperand_ptr = (*ir_vector_ptr)[i].get_roperand_ptr();
			if (roperand_ptr != nullptr && ir::IsVarSymbolOperand(roperand_ptr) &&
				visited_var_symbol_names.find(roperand_ptr->ToString()) == visited_var_symbol_names.end()) {
				string var_symbol_name = roperand_ptr->ToString();
				visited_var_symbol_names.insert(var_symbol_name);
				basic_block_ptr->use_set.insert(var_symbol_name);
			}

			Operand* dest_ptr = (*ir_vector_ptr)[i].get_dest_ptr();
			if (dest_ptr != nullptr && ir::IsVarSymbolOperand(dest_ptr) &&
				visited_var_symbol_names.find(dest_ptr->ToString()) == visited_var_symbol_names.end()) {
				string var_symbol_name = dest_ptr->ToString();
				visited_var_symbol_names.insert(var_symbol_name);
				// 全局变量统一在use集合中
				if (var_symbol_name.substr(0, 3) == "_0_" || 
					use_dest_operators_set.find(op) != use_dest_operators_set.end()) {
					basic_block_ptr->use_set.insert(var_symbol_name);
				}
				else if (def_dest_operators_set.find(op) != def_dest_operators_set.end()) {
					basic_block_ptr->def_set.insert(var_symbol_name);
				}
			}
		}
		assert(basic_block_ptr->use_set.size() + basic_block_ptr->def_set.size() == visited_var_symbol_names.size());
	}
}

set<string> set_union(set<string> a, set<string> b) {
	set<string> rlt = a;
	for (auto str : b) {
		rlt.insert(str);
	}
	return rlt;
}

set<string> set_difference(set<string> a, set<string> b) {
	set<string> rlt;
	for (auto str : a) {
		if (!b.count(str)) {
			rlt.insert(str);
		}
	}
	return rlt;
}

void GenInOutSet(shared_ptr<vector<Quaternion>> const ir_vector_ptr, const vector<BasicBlock*> basic_block_ptrs) {
	// 为每个基本块循环计算in和out集合，直到所有基本块的in集合不再产生变化为止
	bool is_finished = false;
	while (!is_finished) {
		is_finished = true;
		for (auto basic_block_ptr : basic_block_ptrs) {
			set<string> out_set;
			for (auto next_basic_block_ptr : basic_block_ptr->next_basic_block_ptrs) {
				// out[B] = out[B] U in[P] 
				out_set = set_union(out_set, next_basic_block_ptr->in_set);
			}
			basic_block_ptr->out_set = out_set;
			// in[B] = use[B] U (out[B] - def[B])
			set<string> in_set = set_union(basic_block_ptr->use_set, set_difference(basic_block_ptr->out_set, basic_block_ptr->def_set));
			if (basic_block_ptr->in_set != in_set) {
				is_finished = false;
				basic_block_ptr->in_set = in_set;
			}
		}
	}
}

void LiveVariableAnalysis(shared_ptr<vector<Quaternion>> const ir_vector_ptr, const vector<BasicBlock*> basic_block_ptrs) {
	GenUseDefSet(ir_vector_ptr, basic_block_ptrs);
	GenInOutSet(ir_vector_ptr, basic_block_ptrs);
}