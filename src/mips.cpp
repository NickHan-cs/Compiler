#include <map>
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <cmath>
#include "mips.h"
#include "basic_block.h"

using namespace std;

extern ofstream mips_file;
extern ofstream mips_file_optim;

const vector<string> all_tmp_regs = { "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9" };
const vector<string> all_glob_regs = { "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7" };
vector<string> avail_tmp_regs;
vector<string> used_tmp_regs;

vector<BasicBlock*> basic_block_ptrs;
unordered_map<int, BasicBlock*> end_ir_idx2basic_block_ptr;

vector<Quaternion> ir_vec;
Quaternion quat;
static int quat_idx = -1;

string cur_func_label = "";

// �������ü�����ȫ�ּĴ��������ȷ����Ҫ����ı����������Ӧ��ȫ�ּĴ���
unordered_map<string, string> var_name2glob_reg;
// ������ȫ�ּĴ����ľֲ����������Ӧ��ȫ�ּĴ���
unordered_map<MipsSymbol*, string> local_mips_sym2glob_reg;
// ��¼Ϊ�����������ʱ�Ĵ���
unordered_map<MipsSymbol*, string> mips_sym2tmp_reg;
// �ɸ���SymbolOperand��name�õ���MipsSymbol����ָ��
unordered_map<string, MipsSymbol*> mips_symbol_ptr_map;

// �������ɵ�mips����
vector<string> mips_code_vec;
// �����������У��ַ�����Ҫ�������(��֤������������)�����Ҫ�ȴ����������ͳһ���
vector<string> str_dec_mips_codes;

void mips::OutputMipsCodeVec() {
	if (MIPS_CODE_OUTPUT) {
		for (string mips_code : mips_code_vec) {
			mips_file << mips_code << endl;
			mips_file_optim << mips_code << endl;
		}
	}
}

void mips::AddMipsCode(string mips_code) { 
	mips_code_vec.push_back(mips_code); 
}

void mips::AddMipsCode(string mips_code, int offset) { 
	mips_code_vec.insert(mips_code_vec.end() + offset, mips_code); 
}

void mips::Translate(shared_ptr<vector<Quaternion>> ir_vector_ptr) {
	// ���ֻ�����
	basic_block_ptrs = DivideBasicBlocks(ir_vector_ptr);
	for (auto basic_block_ptr : basic_block_ptrs) {
		end_ir_idx2basic_block_ptr[basic_block_ptr->end_ir_idx] = basic_block_ptr;
	}
	for (Quaternion ir : *ir_vector_ptr) {
		ir_vec.push_back(ir);
	}
	avail_tmp_regs = all_tmp_regs;
	mips::PlanGlobRegs(ir_vector_ptr);
	// ��������
	mips::AddMipsCode(".data");
	while (!ir_vec.empty() && ir_vec[0].get_op() != OperatorType::LABEL) {
		mips::DataDec();
	}
	for (string str_dec_mips_code : str_dec_mips_codes) {
		mips::AddMipsCode(str_dec_mips_code);
	}
	// ������
	mips::AddMipsCode(".text");
	// ���ȫ�ֱ�����������ȫ�ּĴ������������Ӧ��la����
	for (auto p : mips_symbol_ptr_map) {
		string var_name = p.first;
		if (var_name2glob_reg.find(var_name) != var_name2glob_reg.end()) {
			string glob_reg_name = var_name2glob_reg[var_name];
			// ȫ�������������������ʼ��ַ����
			mips::AddMipsCode("\tla " + glob_reg_name + ", " + var_name);
			if (!p.second->IsArray()) {
				mips::AddMipsCode("\tlw " + glob_reg_name + ", 0(" + glob_reg_name + ")");
			}
		}
	}
	mips::AddMipsCode("\tj _main_Label");
	while (!ir_vec.empty()) {
		quat = ir_vec[0];
		assert(quat.get_op() == OperatorType::LABEL);
		cur_func_label = quat.get_dest_ptr()->ToString();
		ir_vec.erase(ir_vec.begin());
		quat_idx += 1;
		mips::FuncDec();
	}
}

bool CmpBySecEle(const pair<string, int>& a, const pair<string, int>& b) { return a.second > b.second; }

void mips::PlanGlobRegs(shared_ptr<vector<Quaternion>> ir_vector_ptr) {
	// �������ü�����ȫ�ּĴ�������
	map<string, int> ref_cnt_map;
	for (auto ir : *ir_vector_ptr) {
		Operand* dest_ptr = ir.get_dest_ptr();
		if (dest_ptr != nullptr && ir::IsVarSymbolOperand(dest_ptr)) {
			string dest_name = dest_ptr->ToString();
			if (ref_cnt_map.find(dest_name) == ref_cnt_map.end()) {
				ref_cnt_map[dest_name] = 0;
			}
			ref_cnt_map[dest_name] += 1;
		}

		Operand* loperand_ptr = ir.get_loperand_ptr();
		if (loperand_ptr != nullptr && ir::IsVarSymbolOperand(loperand_ptr)) {
			string loperand_name = loperand_ptr->ToString();
			if (ref_cnt_map.find(loperand_name) == ref_cnt_map.end()) {
				ref_cnt_map[loperand_name] = 0;
			}
			ref_cnt_map[loperand_name] += 1;
		}

		Operand* roperand_ptr = ir.get_roperand_ptr();
		if (roperand_ptr != nullptr && ir::IsVarSymbolOperand(roperand_ptr)) {
			string roperand_name = roperand_ptr->ToString();
			if (ref_cnt_map.find(roperand_name) == ref_cnt_map.end()) {
				ref_cnt_map[roperand_name] = 0;
			}
			ref_cnt_map[roperand_name] += 1;
		}
	}
	vector<pair<string, int>> ref_cnt_vec;
	for (auto p : ref_cnt_map) {
		ref_cnt_vec.push_back(p);
	}
	sort(ref_cnt_vec.begin(), ref_cnt_vec.end(), CmpBySecEle);
	for (int i = 0; i < min(all_glob_regs.size(), ref_cnt_vec.size()); i++) {
		var_name2glob_reg[ref_cnt_vec[i].first] = all_glob_regs[i];
	}
}

void mips::DataDec() {
	quat = ir_vec[0];
	OperatorType op = quat.get_op();
	switch (op)
	{
	case OperatorType::VAR: {
		string var_name = quat.get_dest_ptr()->ToString();
		mips_symbol_ptr_map[var_name] = new MipsSymbol(var_name);
		ir_vec.erase(ir_vec.begin());
		quat_idx += 1;
		quat = ir_vec[0];
		int var_value = 0;
		if (quat.get_op() == OperatorType::LOAD_IMM) {
			assert(quat.get_loperand_ptr()->get_operand_type() == OperandType::ImmOperandType);
			var_value = ((ImmeOperand*)quat.get_loperand_ptr())->get_operand_value();
			mips_symbol_ptr_map[var_name]->set_inited(true);
			ir_vec.erase(ir_vec.begin());
			quat_idx += 1;
		}
		mips::AddMipsCode("\t" + var_name + ":\t.word\t" + std::to_string(var_value));
		break;
	}
	case OperatorType::STR_VAR: {
		assert(quat.get_dest_ptr()->get_operand_type() == OperandType::StrOperandType);
		string string_name = ((StringOperand*)quat.get_dest_ptr())->get_operand_name();
		string string_value = ((StringOperand*)quat.get_dest_ptr())->get_operand_value();
		str_dec_mips_codes.push_back("\t" + string_name + ":\t.asciiz\t\"" + string_value + "\"");
		ir_vec.erase(ir_vec.begin());
		quat_idx += 1;
		break;
	}
	case OperatorType::ARR_VAR: {
		string arr_name = quat.get_dest_ptr()->ToString();
		mips_symbol_ptr_map[arr_name] = new MipsSymbol(arr_name, true);
		assert(quat.get_loperand_ptr()->get_operand_type() == OperandType::ImmOperandType);
		int arr_size = ((ImmeOperand*)quat.get_loperand_ptr())->get_operand_value();
		if (quat.get_roperand_ptr() != nullptr) {
			assert(quat.get_roperand_ptr()->get_operand_type() == OperandType::ImmOperandType);
			arr_size = arr_size * ((ImmeOperand*)quat.get_roperand_ptr())->get_operand_value();
		}
		ir_vec.erase(ir_vec.begin());
		quat_idx += 1;
		quat = ir_vec[0];
		if (quat.get_op() != OperatorType::ARR_INIT) {
			mips::AddMipsCode("\t" + arr_name + ":\t.space\t" + std::to_string(4 * arr_size));
			break;
		}
		vector<int> arr_ele_vec;
		while (quat.get_op() == OperatorType::ARR_INIT) {
			assert(quat.get_dest_ptr()->get_operand_type() == OperandType::ImmOperandType);
			arr_ele_vec.push_back(((ImmeOperand*)quat.get_dest_ptr())->get_operand_value());
			ir_vec.erase(ir_vec.begin());
			quat_idx += 1;
			quat = ir_vec[0];
		}
		assert(!arr_ele_vec.empty());
		string arr_ele_str = std::to_string(arr_ele_vec[0]);
		for (int i = 1; i < arr_ele_vec.size(); i++) {
			arr_ele_str += "," + std::to_string(arr_ele_vec[i]);
		}
		mips::AddMipsCode("\t" + arr_name + ":\t.word\t" + arr_ele_str);
		break;
	}
	default:
		break;
	}
}

bool IsFuncDecEnd(int i) {
	quat = ir_vec[i];
	return quat.get_op() == OperatorType::LABEL && quat.get_dest_ptr()->ToString()[0] == '_';
}

void mips::FuncDec() {
	mips::AddMipsCode(cur_func_label + ":");
	// ɨ������������ͳ�Ʋ�����ջ�ռ�
	int needed_sp_spaces = 0;
	// ��¼��Ҫ��sp�洢�ı���
	vector<string> needed_sp_symbols;
	// ������Ӧ����Ե�ַ
	unordered_map<string, int> symbol2addr;
	for (int i = 0; i < ir_vec.size() && !IsFuncDecEnd(i); i++) {
		quat = ir_vec[i];
		OperatorType op = quat.get_op();
		if (op == OperatorType::PARAM || op == OperatorType::VAR || op == OperatorType::ARR_VAR) {
			string symbol_name = quat.get_dest_ptr()->ToString();
			needed_sp_symbols.push_back(symbol_name);
			int sp_spaces = 4;
			// �ж��Ƿ����������
			if (quat.get_loperand_ptr() != nullptr) {
				assert(quat.get_loperand_ptr()->get_operand_type() == OperandType::ImmOperandType);
				sp_spaces = sp_spaces * ((ImmeOperand*)quat.get_loperand_ptr())->get_operand_value();
				if (quat.get_roperand_ptr() != nullptr) {
					assert(quat.get_roperand_ptr()->get_operand_type() == OperandType::ImmOperandType);
					sp_spaces = sp_spaces * ((ImmeOperand*)quat.get_roperand_ptr())->get_operand_value();
				}
				mips_symbol_ptr_map[symbol_name] = new MipsSymbol(symbol_name, true);
			}
			else {
				mips_symbol_ptr_map[symbol_name] = new MipsSymbol(symbol_name);
				if (op == OperatorType::PARAM) {
					mips_symbol_ptr_map[symbol_name]->set_inited(true);
				}
			}
			needed_sp_spaces += sp_spaces;
			symbol2addr[symbol_name] = needed_sp_spaces;
		}
		else {
			// �����м����ʱ����ʱ�Ĵ���Ҳ����������ͨ����
			if (quat.get_dest_ptr() != nullptr && quat.get_dest_ptr()->IsTempOperand()) {
				string tmp_reg = quat.get_dest_ptr()->ToString();
				if (symbol2addr.find(tmp_reg) == symbol2addr.end()) {
					needed_sp_symbols.push_back(tmp_reg);
					needed_sp_spaces += 4;
					symbol2addr[tmp_reg] = needed_sp_spaces;
					// ��һ�������е���ʱ�Ĵ����Ḳ��ǰһ��������ͬ����ʱ�Ĵ���
					mips_symbol_ptr_map[tmp_reg] = new MipsSymbol(tmp_reg);
				}
			}

			if (quat.get_loperand_ptr() != nullptr && quat.get_loperand_ptr()->IsTempOperand()) {
				string tmp_reg = quat.get_loperand_ptr()->ToString();
				if (symbol2addr.find(tmp_reg) == symbol2addr.end()) {
					needed_sp_symbols.push_back(tmp_reg);
					needed_sp_spaces += 4;
					symbol2addr[tmp_reg] = needed_sp_spaces;
					// ��һ�������е���ʱ�Ĵ����Ḳ��ǰһ��������ͬ����ʱ�Ĵ���
					mips_symbol_ptr_map[tmp_reg] = new MipsSymbol(tmp_reg);
				}
			}

			if (quat.get_roperand_ptr() != nullptr && quat.get_roperand_ptr()->IsTempOperand()) {
				string tmp_reg = quat.get_roperand_ptr()->ToString();
				if (symbol2addr.find(tmp_reg) == symbol2addr.end()) {
					needed_sp_symbols.push_back(tmp_reg);
					needed_sp_spaces += 4;
					symbol2addr[tmp_reg] = needed_sp_spaces;
					// ��һ�������е���ʱ�Ĵ����Ḳ��ǰһ��������ͬ����ʱ�Ĵ���
					mips_symbol_ptr_map[tmp_reg] = new MipsSymbol(tmp_reg);
				}
			}
		}
	}

	mips::AddMipsCode("\tsubi $sp, $sp, " + to_string(needed_sp_spaces));
	for (auto needed_sp_symbol : needed_sp_symbols) {
		mips_symbol_ptr_map[needed_sp_symbol]->set_symbol_relative_addr(needed_sp_spaces - symbol2addr[needed_sp_symbol]);
	}

	while (!ir_vec.empty() && !IsFuncDecEnd(0)) {
		quat = ir_vec[0];
		ir_vec.erase(ir_vec.begin());
		quat_idx += 1;
		OperatorType op = quat.get_op();
		if (op == OperatorType::PARAM || op == OperatorType::VAR || op == OperatorType::ARR_VAR) {
			string symbol_name = quat.get_dest_ptr()->ToString();
			if (var_name2glob_reg.find(symbol_name) != var_name2glob_reg.end()) {
				// �Ѿ��ƻ�Ϊsymbol_name����ȫ�ּĴ���
				string glob_reg_name = var_name2glob_reg[symbol_name];
				MipsSymbol* mips_sym = mips_symbol_ptr_map[symbol_name];
				if (local_mips_sym2glob_reg.find(mips_sym) == local_mips_sym2glob_reg.end()) {
					local_mips_sym2glob_reg[mips_sym] = glob_reg_name;
					int mips_sym_addr = mips_sym->get_symbol_relative_addr();
					if (mips_sym->IsArray()) {
						mips::AddMipsCode("\taddiu " + glob_reg_name + ", $sp, " + to_string(mips_sym_addr));
					}
					else if (op == OperatorType::PARAM){
						mips::AddMipsCode("\tlw " + glob_reg_name + ", " + to_string(mips_sym_addr) + "($sp)");
					}
				}
			}
			continue;
		}

		Operand* loperand_ptr = quat.get_loperand_ptr();
		if (loperand_ptr != nullptr && 
			mips_symbol_ptr_map.find(loperand_ptr->ToString()) != mips_symbol_ptr_map.end()) {
			loperand_ptr = new SymbolOperand(mips::GenReg(loperand_ptr->ToString(), true));
		}

		Operand* roperand_ptr = quat.get_roperand_ptr();
		if (roperand_ptr != nullptr &&
			mips_symbol_ptr_map.find(roperand_ptr->ToString()) != mips_symbol_ptr_map.end()) {
			roperand_ptr = new SymbolOperand(mips::GenReg(roperand_ptr->ToString(), true));
		}

		Operand* dest_ptr = quat.get_dest_ptr();
		if (dest_ptr != nullptr &&
			mips_symbol_ptr_map.find(dest_ptr->ToString()) != mips_symbol_ptr_map.end()) {
			if (def_dest_operators_set.find(op) != def_dest_operators_set.end()) {
				mips_symbol_ptr_map[dest_ptr->ToString()]->set_dirty(true);
			}
			bool is_used = use_dest_operators_set.find(op) != use_dest_operators_set.end();
			dest_ptr = new SymbolOperand(mips::GenReg(dest_ptr->ToString(), is_used));
		}
	
		quat = Quaternion(op, dest_ptr, loperand_ptr, roperand_ptr);

		if (op == OperatorType::ADDU || op == OperatorType::SUBU ||
			op == OperatorType::MUL || op == OperatorType::DIVI) {
			mips::GenArithCode(quat);
		}

		else if (op == OperatorType::BEQ || op == OperatorType::BGE || op == OperatorType::BGT ||
			op == OperatorType::BLE || op == OperatorType::BLT || op == OperatorType::BNE) {
			mips::GenBranchCode(quat);
		}

		else if (op == OperatorType::GOTO) {
			mips::AddMipsCode("\tj " + quat.get_dest_ptr()->ToString());
		}

		else if (op == OperatorType::LABEL) {
			mips::AddMipsCode(quat.get_dest_ptr()->ToString() + ":");
		}

		else if (op == OperatorType::CALL) {
			mips::FuncCall();
		}

		else if (op == OperatorType::RET) {
			if (cur_func_label == "_main_Label") {
				mips::AddMipsCode("\tli $v0, 10");
				mips::AddMipsCode("\tsyscall");
			}
			else {
				if (loperand_ptr != nullptr) {
					string op_name = loperand_ptr->get_operand_type() == OperandType::ImmOperandType ? "li" : "move";
					mips::AddMipsCode("\t" + op_name + " $v0, " + loperand_ptr->ToString());
				}
				for (auto p : mips_sym2tmp_reg) {
					MipsSymbol* mips_sym = p.first;
					if (!mips_sym->IsArray() && mips_sym->get_symbol_relative_addr() == -1) {
						mips::AddMipsCode("\tla $a0, " + mips_sym->get_symbol_name());
						mips::AddMipsCode("\tsw " + p.second + ", 0($a0)");
					}
				}
				mips::AddMipsCode("\taddiu $sp, $sp, " + to_string(needed_sp_spaces));
				mips::AddMipsCode("\tjr $ra");
			}
		}

		else if (op == OperatorType::MV) {
			string dest_name = quat.get_dest_ptr()->ToString();
			if (dest_name[1] == 'a') {
				mips::FuncCall();
			}
			else {
				Operand* loperand_ptr = quat.get_loperand_ptr();
				string op_name = loperand_ptr->get_operand_type() == OperandType::ImmOperandType ? "li" : "move";
				mips::AddMipsCode("\t" + op_name + " " + dest_name + ", " + loperand_ptr->ToString());
			}
		}
		
		else if (op == OperatorType::STORE || op == OperatorType::ARR_INIT) {
			Operand* roperand_ptr = quat.get_roperand_ptr();
			string offset = "0";
			string base_addr = quat.get_loperand_ptr()->ToString();
			if (roperand_ptr->get_operand_type() == OperandType::ImmOperandType) {
				offset = to_string(((ImmeOperand*)roperand_ptr)->get_operand_value() * 4);
			}
			else {
				mips::AddMipsCode("\tsll $a0, " + roperand_ptr->ToString() + ", 2");
				mips::AddMipsCode("\taddu $a0, $a0, " + base_addr);
				base_addr = "$a0";
			}
			Operand* dest_ptr = quat.get_dest_ptr();
			string dest_name = dest_ptr->ToString();
			if (dest_ptr->get_operand_type() == OperandType::ImmOperandType) {
				mips::AddMipsCode("\tli $a1, " + dest_name);
				dest_name = "$a1";
			}
			mips::AddMipsCode("\tsw " + dest_name + ", " + offset + "(" + base_addr + ")");
		}
		
		else if (op == OperatorType::LOAD) {
			Operand* roperand_ptr = quat.get_roperand_ptr();
			string offset = "0";
			string base_addr = quat.get_loperand_ptr()->ToString();
			if (roperand_ptr->get_operand_type() == OperandType::ImmOperandType) {
				offset = to_string(((ImmeOperand*)roperand_ptr)->get_operand_value() * 4);
			}
			else {
				mips::AddMipsCode("\tsll $a0, " + roperand_ptr->ToString() + ", 2");
				mips::AddMipsCode("\taddu $a0, $a0, " + base_addr);
				base_addr = "$a0";
			}
			mips::AddMipsCode("\tlw " + quat.get_dest_ptr()->ToString() + ", " + offset + "(" + base_addr + ")");
		}
		
		else if (op == OperatorType::LOAD_IMM) {
			mips::AddMipsCode("\tli " + quat.get_dest_ptr()->ToString() + ", " + quat.get_loperand_ptr()->ToString());
		}
		
		else if (op == OperatorType::READ_INT || op == OperatorType::READ_CHAR) {
			string syscall_idx = op == OperatorType::READ_CHAR ? "12" : "5";
			mips::AddMipsCode("\tli $v0, " + syscall_idx);
			mips::AddMipsCode("\tsyscall");
			mips::AddMipsCode("\tmove " + quat.get_dest_ptr()->ToString() + ", $v0");
		}
		
		else if (op == OperatorType::PRINT_INT || op == OperatorType::PRINT_CHAR || op == OperatorType::PRINT_STRING) {
			Operand* dest_ptr = quat.get_dest_ptr();
			string dest_name = dest_ptr->ToString();
			if (op == OperatorType::PRINT_STRING) {
				dest_name = ((StringOperand*)dest_ptr)->get_operand_name();
				if (dest_name == "enter_str") {
					mips::AddMipsCode("\tli $a0, 10");
					mips::AddMipsCode("\tli $v0, 11");
				}
				else {
					dest_name = ((StringOperand*)dest_ptr)->get_operand_name();
					mips::AddMipsCode("\tla $a0, " + dest_name);
					mips::AddMipsCode("\tli $v0, 4");
				}
			}
			else {
				string instr_name = dest_ptr->get_operand_type() == OperandType::ImmOperandType ? "li" : "move";
				mips::AddMipsCode("\t" + instr_name + " $a0, " + dest_name);
				string syscall_idx = op == OperatorType::PRINT_CHAR ? "11" : "1";
				mips::AddMipsCode("\tli $v0, " + syscall_idx);
			}
			mips::AddMipsCode("\tsyscall");
		}
	
		if (end_ir_idx2basic_block_ptr.find(quat_idx) != end_ir_idx2basic_block_ptr.end() && 
			op != OperatorType::RET && op != OperatorType::CALL) {
			for (auto p : mips_sym2tmp_reg) {
				MipsSymbol* mips_sym = p.first;
				string tmp_reg_name = p.second;
				if (!mips_sym->IsArray()) {
					int mips_sym_addr = mips_sym->get_symbol_relative_addr();
					string mips_sym_name = mips_sym->get_symbol_name();
					if (mips_sym_addr == -1) {
						mips::AddMipsCode("\tla $a0, " + mips_sym_name, -1);
						mips::AddMipsCode("\tsw " + tmp_reg_name + ", 0($a0)", -1);
					}
					else if (mips_sym->IsDirty() && 
						end_ir_idx2basic_block_ptr[quat_idx]->out_set.find(mips_sym_name) !=
						end_ir_idx2basic_block_ptr[quat_idx]->out_set.end()) {
						mips::AddMipsCode("\tsw " + tmp_reg_name + ", " + to_string(mips_sym_addr) + "($sp)", -1);
					}
					mips_sym->set_inited(true);
					mips_sym->set_dirty(false);
				}
			}
			mips_sym2tmp_reg.clear();
			avail_tmp_regs = all_tmp_regs;
			used_tmp_regs = vector<string>();
		}
	}

	if (cur_func_label == "_main_Label") {
		mips::AddMipsCode("\tli $v0, 10");
		mips::AddMipsCode("\tsyscall");
	}
	else {
		for (auto symbol : needed_sp_symbols) {
			mips_symbol_ptr_map.erase(symbol);
		}
		// ��շ�����ȫ�ּĴ����ľֲ�����
		local_mips_sym2glob_reg.clear();
		// ���˳�����ǰ��Ҫ����������ʱ�Ĵ�����ȫ�ּ򵥱�������(�ֲ�������ȫ�������������Ҫ����)
		for (auto p : mips_sym2tmp_reg) {
			MipsSymbol* mips_sym = p.first;
			string tmp_reg_name = p.second;
			if (!mips_sym->IsArray()) {
				if (mips_sym->get_symbol_relative_addr() == -1) {
					mips::AddMipsCode("\tla $a0, " + mips_sym->get_symbol_name());
					mips::AddMipsCode("\tsw " + tmp_reg_name + ", 0($a0)");
				}
				mips_sym->set_inited(true);
				mips_sym->set_dirty(false);
			}
		}
		// ��շ�������ʱ�Ĵ��������б���
		mips_sym2tmp_reg.clear();
		avail_tmp_regs = all_tmp_regs;
		used_tmp_regs = vector<string>();
		mips::AddMipsCode("\taddiu $sp, $sp, " + to_string(needed_sp_spaces));
		mips::AddMipsCode("\tjr $ra");
	}
}

string mips::GenReg(string var_name, bool is_used) {
	MipsSymbol* mips_sym = mips_symbol_ptr_map[var_name];

	if (var_name2glob_reg.find(var_name) != var_name2glob_reg.end()) {
		// ����Ѿ��ƻ�Ϊvar_name����ȫ�ּĴ���
		string glob_reg_name = var_name2glob_reg[var_name];
		int mips_sym_addr = mips_sym->get_symbol_relative_addr();
		// ����Ǳ�����ȫ�ּĴ������Ǿֲ����������ǵ�һ������(���ñ�����δ���뵽local_mips_sym2glob_reg��)
		if (mips_sym_addr != -1 && local_mips_sym2glob_reg.find(mips_sym) == local_mips_sym2glob_reg.end()) {
			local_mips_sym2glob_reg[mips_sym] = glob_reg_name;
			if (mips_sym->IsArray()) {
				// ����Ǿֲ�����������ͽ��������ʼ��ַ���ص�ȫ�ּĴ�����
				mips::AddMipsCode("\taddiu " + glob_reg_name + ", $sp, " + to_string(mips_sym_addr));
			}
			else if (is_used) {
				// ����Ǽ򵥱������ڵ�ǰ�������Ǳ�ʹ�õģ��ͽ��򵥱�����ֵ���ص��Ĵ�����
				mips::AddMipsCode("\tlw " + glob_reg_name + ", " + to_string(mips_sym_addr) + "($sp)");
			}
		}

		return glob_reg_name;
	}

	if (mips_sym2tmp_reg.find(mips_sym) == mips_sym2tmp_reg.end()) {
		// �����δΪmips_sym������ʱ�Ĵ���
		string tmp_reg_name = mips::PlanTmpReg(mips_sym);
		assert(tmp_reg_name != "");
		mips_sym2tmp_reg[mips_sym] = tmp_reg_name;
		int mips_sym_addr = mips_sym->get_symbol_relative_addr();
		if (mips_sym->IsArray()) {
			if (mips_sym_addr == -1) {
				// ���mips_sym��ȫ������
				mips::AddMipsCode("\tla " + tmp_reg_name + ", " + var_name);
			}
			else {
				// ���mips_sym�Ǿֲ�����
				mips::AddMipsCode("\taddiu " + tmp_reg_name + ", $sp, " + to_string(mips_sym_addr));
			}
		}
		else if (is_used && (mips_sym_addr == -1 || mips_sym->IsInited())) {
			// ���mips_sym�Ǽ򵥱����ҵ�ǰ����������use_dest_operators_set
			// ��Ϊ�����������Ǳ���ֵ�Ļ�������Ҫȥ������ԭ�ȵ�ֵ
			if (mips_sym_addr == -1) {
				// ���mips_sym��ȫ�ּ򵥱���
				mips::AddMipsCode("\tla $a0, " + var_name);
				mips::AddMipsCode("\tlw " + tmp_reg_name + ", 0($a0)");
			}
			else {
				// ���mips_sym�Ǿֲ��򵥱���
				mips::AddMipsCode("\tlw " + tmp_reg_name + ", " + to_string(mips_sym_addr) + "($sp)");
			}
		}
	}

	return mips_sym2tmp_reg[mips_sym];
}

string mips::PlanTmpReg(MipsSymbol* mips_sym) {
	if (!avail_tmp_regs.empty()) {
		string tmp_reg_name = avail_tmp_regs[0];
		avail_tmp_regs.erase(avail_tmp_regs.begin());
		used_tmp_regs.push_back(tmp_reg_name);
		return tmp_reg_name;
	}
	for (auto iter = used_tmp_regs.begin(); iter != used_tmp_regs.end(); iter++) {
		// Ҫ��֤�������ʱ�Ĵ����������ڴ�����������Ҳ������ļĴ���
		if (quat.get_loperand_ptr() != nullptr) {
			MipsSymbol* loperand_sym = mips_symbol_ptr_map[quat.get_loperand_ptr()->ToString()];
			if (mips_sym2tmp_reg.find(loperand_sym) != mips_sym2tmp_reg.end() && 
				mips_sym2tmp_reg[loperand_sym] == *iter) {
				continue;
			}
		}

		if (quat.get_roperand_ptr() != nullptr) {
			MipsSymbol* roperand_sym = mips_symbol_ptr_map[quat.get_roperand_ptr()->ToString()];
			if (mips_sym2tmp_reg.find(roperand_sym) != mips_sym2tmp_reg.end() &&
				mips_sym2tmp_reg[roperand_sym] == *iter) {
				continue;
			}
		}

		string tmp_reg_name = *iter;
		used_tmp_regs.erase(iter);
		// �ͷŸüĴ�������ԭ�ȴ洢��ֵ����ڴ�
		for (auto iter = mips_sym2tmp_reg.begin(); iter != mips_sym2tmp_reg.end(); iter++) {
			if (iter->second == tmp_reg_name) {
				MipsSymbol* mips_sym = iter->first;
				// �����ڼĴ����еĴ������ʼ��ַ������Ҫ����
				if (!mips_sym->IsArray()) {
					int mips_sym_addr = mips_sym->get_symbol_relative_addr();
					if (mips_sym_addr == -1) {
						// ����ȫ�ּ򵥱���
						mips::AddMipsCode("\tla $a0, " + mips_sym->get_symbol_name());
						mips::AddMipsCode("\tsw " + tmp_reg_name + ", 0($a0)");
					}
					else {
						mips::AddMipsCode("\tsw " + tmp_reg_name + ", " + to_string(mips_sym_addr) + "($sp)");
					}
					mips_sym->set_inited(true);
					mips_sym->set_dirty(false);
				}
				mips_sym2tmp_reg.erase(iter);
				break;
			}
		}
		used_tmp_regs.push_back(tmp_reg_name);
		return tmp_reg_name;
	}
	return "";
}

void mips::GenArithCode(Quaternion quat) {
	OperatorType op = quat.get_op();
	Operand* dest_ptr = quat.get_dest_ptr();
	string dest_name = dest_ptr->ToString();
	Operand* loperand_ptr = quat.get_loperand_ptr();
	OperandType loperand_type = loperand_ptr->get_operand_type();
	Operand* roperand_ptr = quat.get_roperand_ptr();
	OperandType roperand_type = roperand_ptr->get_operand_type();
	if (loperand_type == OperandType::ImmOperandType && roperand_type == OperandType::ImmOperandType) {
		int loperand_value = ((ImmeOperand*)loperand_ptr)->get_operand_value();
		int roperand_value = ((ImmeOperand*)roperand_ptr)->get_operand_value();
		int dest_value = 0;
		switch (op)
		{
		case OperatorType::ADDU: {
			dest_value = loperand_value + roperand_value;
			break;
		}
		case OperatorType::SUBU: {
			dest_value = loperand_value - roperand_value;
			break;
		}
		case OperatorType::MUL: {
			dest_value = loperand_value * roperand_value;
			break;
		}
		case OperatorType::DIVI: {
			dest_value = loperand_value / roperand_value;
			break;
		}
		default:
			break;
		}
		mips::AddMipsCode("\tli " + dest_name + ", " + to_string(dest_value));
	}
	else if (loperand_type == OperandType::ImmOperandType) {
		int loperand_value = ((ImmeOperand*)loperand_ptr)->get_operand_value();
		string roperand_name = roperand_ptr->ToString();
		switch (op)
		{
		case OperatorType::ADDU: {
			mips::AddMipsCode("\taddiu " + dest_name + ", " + roperand_name + ", " + to_string(loperand_value));
			break;
		}
		case OperatorType::SUBU: {
			if (loperand_value != 0) {
				mips::AddMipsCode("\taddiu " + dest_name + ", " + roperand_name + ", " + to_string(-loperand_value));
				mips::AddMipsCode("\tneg " + dest_name + ", " + dest_name);
			}
			else {
				mips::AddMipsCode("\tneg " + dest_name + ", " + roperand_name);
			}
			
			break;
		}
		case OperatorType::MUL: {
			if (MIPS_CODE_OPTIMIZE) {
				if (loperand_value == 0) {
					mips::AddMipsCode("\tli " + dest_name + ", 0");
					break;
				}
				else if (loperand_value == -1) {
					mips::AddMipsCode("\tneg " + dest_name + ", " + roperand_name);
					break;
				}
				else if (!(abs(loperand_value) & (abs(loperand_value) - 1))) {
					// loperand_value��2����������
					int log_ans = (int)(log2(abs(loperand_value)));
					mips::AddMipsCode("\tsll " + dest_name + ", " + roperand_name + ", " + to_string(log_ans));
					if (loperand_value < 0) {
						mips::AddMipsCode("\tneg " + dest_name + ", " + dest_name);
					}
					break;
				}
			}
			mips::AddMipsCode("\tmul " + dest_name + ", " + roperand_name + ", " + to_string(loperand_value));
			break;
		}
		case OperatorType::DIVI: {
			if (loperand_value == 0) {
				mips::AddMipsCode("\tli " + dest_name + ", 0");
			}
			else {
				mips::AddMipsCode("\tli $a0, " + to_string(loperand_value));
				mips::AddMipsCode("\tdiv $a0, " + roperand_name);
				mips::AddMipsCode("\tmflo " + dest_name);
			}
			break;
		}
		default:
			break;
		}
	}
	else if (roperand_type == OperandType::ImmOperandType) {
		string loperand_name = loperand_ptr->ToString();
		int roperand_value = ((ImmeOperand*)roperand_ptr)->get_operand_value();
		switch (op)
		{
		case OperatorType::ADDU: {
			mips::AddMipsCode("\taddiu " + dest_name + ", " + loperand_name + ", " + to_string(roperand_value));
			break;
		}
		case OperatorType::SUBU: {
			mips::AddMipsCode("\taddiu " + dest_name + ", " + loperand_name + ", " + to_string(-roperand_value));
			break;
		}
		case OperatorType::MUL: {
			if (MIPS_CODE_OPTIMIZE) {
				if (roperand_value == 0) {
					mips::AddMipsCode("\tli " + dest_name + ", 0");
					break;
				}
				else if (roperand_value == -1) {
					mips::AddMipsCode("\tneg " + dest_name + ", " + loperand_name);
					break;
				} 
				else if (!(abs(roperand_value) & (abs(roperand_value) - 1))) {
					// roperand_value��2����������
					int log_ans = (int)(log2(abs(roperand_value)));
					mips::AddMipsCode("\tsll " + dest_name + ", " + loperand_name + ", " + to_string(log_ans));
					if (roperand_value < 0) {
						mips::AddMipsCode("\tneg " + dest_name + ", " + dest_name);
					}
					break;
				}
			}
			mips::AddMipsCode("\tmul " + dest_name + ", " + loperand_name + ", " + to_string(roperand_value));
			break;
		}
		case OperatorType::DIVI: {
			if (MIPS_CODE_OPTIMIZE) {
				if (roperand_value == -1) {
					mips::AddMipsCode("\tneg " + dest_name + ", " + loperand_name);
					break;
				}
				else if (!(abs(roperand_value) & (abs(roperand_value) - 1))) {
					// roperand_value��2����������
					int log_ans = (int)(log2(abs(roperand_value)));
					mips::AddMipsCode("\tmove $a0, " + loperand_name);
					mips::AddMipsCode("\taddiu $a1, " + loperand_name + ", " + to_string((abs(roperand_value) - 1)));
					mips::AddMipsCode("\tslt $a2, " + loperand_name + ", $0");
					mips::AddMipsCode("\tmovn $a0, $a1, $a2");
					mips::AddMipsCode("\tsra " + dest_name + ", $a0, " + to_string(log_ans));
					if (roperand_value < 0) {
						mips::AddMipsCode("\tneg " + dest_name + ", " + dest_name);
					}
					break;
				}
			}
			mips::AddMipsCode("\tli $a0, " + to_string(roperand_value));
			mips::AddMipsCode("\tdiv " + loperand_name + ", $a0");
			mips::AddMipsCode("\tmflo " + dest_name);
			break;
		}
		default:
			break;
		}
	}
	else {
		string loperand_name = loperand_ptr->ToString();
		string roperand_name = roperand_ptr->ToString();
		switch (op)
		{
		case OperatorType::ADDU: {
			mips::AddMipsCode("\taddu " + dest_name + ", " + loperand_name + ", " + roperand_name);
			break;
		}
		case OperatorType::SUBU: {
			mips::AddMipsCode("\tsubu " + dest_name + ", " + loperand_name + ", " + roperand_name);
			break;
		}
		case OperatorType::MUL: {
			mips::AddMipsCode("\tmul " + dest_name + ", " + loperand_name + ", " + roperand_name);
			break;
		}
		case OperatorType::DIVI: {
			mips::AddMipsCode("\tdiv " + loperand_name + ", " + roperand_name);
			mips::AddMipsCode("\tmflo " + dest_name);
			break;
		}
		default:
			break;
		}
	}
}

void mips::GenBranchCode(Quaternion quat) {
	OperatorType op = quat.get_op();
	string op_name = "";
	switch (op)
	{
	case BLT: {
		op_name = "blt";
		break;
	}
	case BLE: {
		op_name = "ble";
		break;
	}
	case BGT: {
		op_name = "bgt";
		break;
	}
	case BGE: {
		op_name = "bge";
		break;
	}
	case BEQ: {
		op_name = "beq";
		break;
	}
	case BNE: {
		op_name = "bne";
		break;
	}
	default:
		break;
	}
	
	Operand* dest_ptr = quat.get_dest_ptr();
	string dest_name = dest_ptr->ToString();

	Operand* loperand_ptr = quat.get_loperand_ptr();
	OperandType loperand_type = loperand_ptr->get_operand_type();
	string loperand_name = loperand_ptr->ToString();
	if (loperand_type == OperandType::ImmOperandType) {
		mips::AddMipsCode("\tli $a0, " + loperand_name);
		loperand_name = "$a0";
	}
	
	Operand* roperand_ptr = quat.get_roperand_ptr();
	OperandType roperand_type = roperand_ptr->get_operand_type();
	string roperand_name = roperand_ptr->ToString();
	if (roperand_type == OperandType::ImmOperandType) {
		mips::AddMipsCode("\tli $a1, " + roperand_name);
		roperand_name = "$a1";
	}

	mips::AddMipsCode("\t" + op_name + " " + loperand_name + ", " + roperand_name + ", " + dest_name);
}

void mips::FuncCall() {
	mips::AddMipsCode("\tsw $ra, -4($sp)");
	while (quat.get_op() != OperatorType::CALL) {
		int offset = -4 * stoi(quat.get_dest_ptr()->ToString().substr(2)) - 8;
		Operand* loperand_ptr = quat.get_loperand_ptr();
		string loperand_name = loperand_ptr->ToString();
		if (loperand_ptr->get_operand_type() == OperandType::ImmOperandType) {
			mips::AddMipsCode("\tli $a0, " + loperand_name);
			mips::AddMipsCode("\tsw $a0, " + to_string(offset) + "($sp)");
		}
		else {
			mips::AddMipsCode("\tsw " + loperand_name + ", " + to_string(offset) + "($sp)");
		}
		quat = ir_vec[0];
		ir_vec.erase(ir_vec.begin());
		quat_idx += 1;
		// opֻ��MOVE��CALL�������MOVE��ֻ�������������Ҫ����Ĵ���
		OperatorType op = quat.get_op();
		loperand_ptr = quat.get_loperand_ptr();
		if (loperand_ptr != nullptr &&
			mips_symbol_ptr_map.find(loperand_ptr->ToString()) != mips_symbol_ptr_map.end()) {
			loperand_ptr = new SymbolOperand(mips::GenReg(loperand_ptr->ToString(), true));
		}
		Operand* roperand_ptr = quat.get_roperand_ptr();
		Operand* dest_ptr = quat.get_dest_ptr();
		quat = Quaternion(op, dest_ptr, loperand_ptr, roperand_ptr);
	}
	// ��žֲ�������ȫ�ּĴ����ڵ��ú���ǰ��Ҫ����
	for (auto p : local_mips_sym2glob_reg) {
		MipsSymbol* local_mips_sym = p.first;
		string glob_reg_name = p.second;
		if (!local_mips_sym->IsArray()) {
			mips::AddMipsCode("\tsw " + glob_reg_name + ", " + 
				to_string(local_mips_sym->get_symbol_relative_addr()) + "($sp)");
		}
	}

	for (auto p : mips_sym2tmp_reg) {
		MipsSymbol* mips_sym = p.first;
		string tmp_reg_name = p.second;
		if (!mips_sym->IsArray()) {
			int mips_sym_addr = mips_sym->get_symbol_relative_addr();
			if (mips_sym_addr == -1) {
				mips::AddMipsCode("\tla $a0, " + mips_sym->get_symbol_name());
				mips::AddMipsCode("\tsw " + tmp_reg_name + ", 0($a0)");
			}
			else if (mips_sym->IsDirty()) {
				mips::AddMipsCode("\tsw " + tmp_reg_name + ", " + to_string(mips_sym_addr) + "($sp)");
			}
			mips_sym->set_inited(true);
			mips_sym->set_dirty(false);
		}
	}
	mips_sym2tmp_reg.clear();
	avail_tmp_regs = all_tmp_regs;
	used_tmp_regs = vector<string>();

	mips::AddMipsCode("\taddiu $sp, $sp, -4");
	mips::AddMipsCode("\tjal " + quat.get_dest_ptr()->ToString());
	mips::AddMipsCode("\tlw $ra, 0($sp)");
	mips::AddMipsCode("\taddiu $sp, $sp, 4");
	
	for (auto p : local_mips_sym2glob_reg) {
		MipsSymbol* local_mips_sym = p.first;
		string local_mips_sym_addr = to_string(local_mips_sym->get_symbol_relative_addr());
		string glob_reg_name = p.second;
		if (local_mips_sym->IsArray()) {
			mips::AddMipsCode("\taddiu " + glob_reg_name + ", $sp, " + local_mips_sym_addr);
		}
		else {
			mips::AddMipsCode("\tlw " + glob_reg_name + ", " + local_mips_sym_addr + "($sp)");
		}
	}
}
