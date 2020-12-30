#include <memory>
#include <vector>
#include <stack>
#include <string>
#include <fstream>
#include <unordered_map>
#include <cassert>
#include "ir.h"
#include "symbol.h"
#include "basic_block.h"

using namespace std;

extern ofstream ir_file;

static int string_idx = 0;
static int temp_idx = -1;


stack<Operand*> avail_temp_operand_ptrs;

unordered_map<shared_ptr<Symbol>, SymbolOperand*> symbol_operand_map;
//	所有四元式的vector
shared_ptr<vector<Quaternion>> quat_vector_ptr = make_shared<vector<Quaternion>>();

bool ir::IsVarSymbolOperand(Operand* operand_ptr) {
	return operand_ptr->get_operand_type() == OperandType::SymOperandType &&
		operand_ptr->ToString()[1] != 'a' && operand_ptr->ToString()[1] != 'v';
}

SymbolOperand* ir::GetSymbolOperandPtr(shared_ptr<Symbol> symbol_ptr) {
	if (symbol_operand_map.find(symbol_ptr) == symbol_operand_map.end()) {
		SymbolOperand* symbol_operand_ptr = new SymbolOperand("_" + to_string(symbol_ptr->get_symbol_level()) + "_" + symbol_ptr->get_symbol_name());
		symbol_operand_map.insert(make_pair(symbol_ptr, symbol_operand_ptr));
	}
	return symbol_operand_map.at(symbol_ptr);
}

shared_ptr<vector<Quaternion>> ir::GetQuatVectorPtr() {
	return quat_vector_ptr;
}

void ir::AddQuaternionBack(Quaternion quat) {
	quat_vector_ptr->push_back(quat);
	if ((quat.get_op() == OperatorType::STORE || quat.get_op() == OperatorType::PRINT_INT ||
		quat.get_op() == OperatorType::PRINT_CHAR) &&
		quat.get_dest_ptr() != nullptr && quat.get_dest_ptr()->IsTempOperand()) {
		avail_temp_operand_ptrs.push(quat.get_dest_ptr());
	}
	if (quat.get_loperand_ptr() != nullptr && quat.get_loperand_ptr()->IsTempOperand()) {
		avail_temp_operand_ptrs.push(quat.get_loperand_ptr());
	}
	if (quat.get_roperand_ptr() != nullptr && quat.get_roperand_ptr()->IsTempOperand()) {
		avail_temp_operand_ptrs.push(quat.get_roperand_ptr());
	}
}

void ir::AddQuaternionHead(Quaternion quat) {
	quat_vector_ptr->insert(quat_vector_ptr->begin(), quat);
}

string ir::ProdStringName() {
	string_idx += 1;
	return "str_" + to_string(string_idx);
}

Operand* ir::ProdTempOperandPtr() {
	string temp_symbol_name = "";
	if (avail_temp_operand_ptrs.empty()) {
		temp_idx += 1;
		assert(temp_idx <= 9);
		SymbolOperand* temp_operand_ptr = new SymbolOperand("$t" + to_string(temp_idx), true);
		return temp_operand_ptr;
	}
	Operand* temp_operand_ptr = avail_temp_operand_ptrs.top();
	avail_temp_operand_ptrs.pop();
	return temp_operand_ptr;
}

void ir::OutputQuatVector() {
	if (IR_OUTPUT) {
		int quat_vector_size = quat_vector_ptr->size();
		for (int i = 0; i < quat_vector_size; i++) {
			ir_file << quat_vector_ptr->at(i).ToString() << endl;
		}
	}
}

void ir::IrOptim() {
	if (IR_OPTIMIZE) {
		ir::DelDeadCode();
	}
}

void ir::DelDeadCode() {
	while (true) {
		vector<BasicBlock*> basic_block_ptrs = DivideBasicBlocks(quat_vector_ptr);
		set<int> dead_code_idxs = GetDeadCodes(quat_vector_ptr, basic_block_ptrs);
		if (dead_code_idxs.empty()) {
			break;
		}
		shared_ptr<vector<Quaternion>> new_quat_vector_ptr = make_shared<vector<Quaternion>>();
		for (int i = 0; i < quat_vector_ptr->size(); i++) {
			if (dead_code_idxs.find(i) == dead_code_idxs.end()) {
				new_quat_vector_ptr->push_back((*quat_vector_ptr)[i]);
			}
		}
		quat_vector_ptr = make_shared<vector<Quaternion>>(*new_quat_vector_ptr);
	}
}