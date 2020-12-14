#pragma once
#include "ir.h"

const bool MIPS_CODE_OUTPUT = true;

const bool MIPS_CODE_OPTIMIZE = true;

class MipsSymbol {
private:
	std::string symbol_name;
	// 全局变量的symbol_relative_addr为-1，局部变量的symbol_relative_addr为非负
	// 真实地址 = $sp + 相对地址
	int symbol_relative_addr = -1;
	bool inited = false;
	bool array = false;
	bool dirty = false;
public:
	MipsSymbol(std::string name) { symbol_name = name; }

	MipsSymbol(std::string name, bool array) {
		symbol_name = name;
		this->array = array;
	}

	~MipsSymbol(void) {};

	std::string get_symbol_name() { return symbol_name; }

	void set_symbol_relative_addr(int addr) { this->symbol_relative_addr = addr; }
	
	int get_symbol_relative_addr() { return symbol_relative_addr; }

	bool IsInited() { return inited; }

	void set_inited(bool inited) { this->inited = inited; }

	bool IsArray() { return array; }

	void set_dirty(bool dirty) { this->dirty = dirty; }

	bool IsDirty() { return dirty; }
};

namespace mips {
	void Translate(std::shared_ptr<std::vector<Quaternion>> ir_vector_ptr);
	
	// 得到全局寄存器分配方案(尚未真正分配)
	void PlanGlobRegs(std::shared_ptr<std::vector<Quaternion>> ir_vector_ptr);

	void OutputMipsCodeVec();

	// 添加mips代码
	void AddMipsCode(std::string mips_code);

	void AddMipsCode(std::string mips_code, int offset);

	// 数据声明
	void DataDec();

	// 函数声明
	void FuncDec();

	std::string GenReg(std::string var_name, bool);

	// 为mips_sym计划分配的临时寄存器
	std::string PlanTmpReg(MipsSymbol* mips_sym);

	void GenArithCode(Quaternion quat);

	void GenBranchCode(Quaternion quat);

	void FuncCall();
}