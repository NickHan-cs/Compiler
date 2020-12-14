#pragma once
#include "ir.h"

const bool MIPS_CODE_OUTPUT = true;

const bool MIPS_CODE_OPTIMIZE = true;

class MipsSymbol {
private:
	std::string symbol_name;
	// ȫ�ֱ�����symbol_relative_addrΪ-1���ֲ�������symbol_relative_addrΪ�Ǹ�
	// ��ʵ��ַ = $sp + ��Ե�ַ
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
	
	// �õ�ȫ�ּĴ������䷽��(��δ��������)
	void PlanGlobRegs(std::shared_ptr<std::vector<Quaternion>> ir_vector_ptr);

	void OutputMipsCodeVec();

	// ���mips����
	void AddMipsCode(std::string mips_code);

	void AddMipsCode(std::string mips_code, int offset);

	// ��������
	void DataDec();

	// ��������
	void FuncDec();

	std::string GenReg(std::string var_name, bool);

	// Ϊmips_sym�ƻ��������ʱ�Ĵ���
	std::string PlanTmpReg(MipsSymbol* mips_sym);

	void GenArithCode(Quaternion quat);

	void GenBranchCode(Quaternion quat);

	void FuncCall();
}