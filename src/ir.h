#pragma once
#include<set>
#include <string>
#include <memory>
#include <unordered_map>
#include "symbol.h"

const bool IR_OUTPUT = true;

const bool IR_OPTIMIZE = true;

enum OperatorType {
	ADDU,		// +
	SUBU,		// -
	MUL,		// *
	DIVI,		// /

	BLT,		// goto dest if loperand <  roperand
	BLE,		// goto dest if loperand <= roperand
	BGT,		// goto dest if loperand >  roperand
	BGE,		// goto dest if loperand >= roperand
	BEQ,		// goto dest if loperand == roperand
	BNE,		// goto dest if loperand != roperand

	GOTO,		// GOTO
	LABEL,		// Label_*	/ main
	CALL,		// call
	RET,		// ret
	
	VAR,		// var
	STR_VAR,	// .asciiz
	ARR_VAR,	// .space / .word
	ARR_INIT,	// 数组初始化
	PARAM,		// param

	MV,			// move
	STORE,		// store
	LOAD,		// load
	LOAD_IMM,		// li

	READ_INT,		// 读入整数
	READ_CHAR,		// 读入字符
	PRINT_INT,		// 输出整数
	PRINT_CHAR,		// 输出字符
	PRINT_STRING,	// 输出字符串

	UNKNOWN_OPERATOR_TYPE,
};

// dest操作数为标识符操作数且是被赋值的操作符集合
const std::set<OperatorType> def_dest_operators_set{
	OperatorType::ADDU,
	OperatorType::SUBU,
	OperatorType::MUL,
	OperatorType::DIVI,
	OperatorType::MV,
	OperatorType::LOAD,
	OperatorType::LOAD_IMM,
	OperatorType::READ_INT,
	OperatorType::READ_CHAR,
};

// dest操作数为标识符操作数且是被使用的操作符集合
const std::set<OperatorType> use_dest_operators_set{
	OperatorType::STORE,
	OperatorType::PRINT_INT,
	OperatorType::PRINT_CHAR,
};

const std::unordered_map<OperatorType, std::string> operator_map{
	{ADDU,			"ADDU"},
	{SUBU,			"SUBU"},
	{MUL,			"MUL"},
	{DIVI,			"DIVI"},

	{BLT,			"BLT"},
	{BLE,			"BLE"},
	{BGT,			"BGT"},
	{BGE,			"BGE"},
	{BEQ,			"BEQ"},
	{BNE,			"BNE"},

	{GOTO,			"GOTO"},
	{LABEL,			"LABEL"},
	{CALL,			"CALL"},
	{RET,			"RET"},

	{VAR,			"VAR"},
	{STR_VAR,		"STR_VAR"},
	{ARR_VAR,		"ARR_VAR"},
	{ARR_INIT,		"ARR_INIT"},
	{PARAM,			"PARAM"},

	{MV,			"MV"},
	{STORE,			"STORE"},
	{LOAD,			"LOAD"},
	{LOAD_IMM,		"LOAD_IMM"},

	{READ_INT,		"READ_INT"},
	{READ_CHAR,		"READ_CHAR"},
	{PRINT_INT,		"PRINT_INT"},
	{PRINT_CHAR,	"PRINT_CHAR"},
	{PRINT_STRING,	"PRINT_STRING"},
	{UNKNOWN_OPERATOR_TYPE,	"UNKNOWN"}
};

// 操作数类型
enum OperandType {
	SymOperandType,
	ImmOperandType,
	StrOperandType,
	LabOperandType,
	OtherOperandType
};

class Operand {
protected:
	OperandType operand_type = OtherOperandType;
public:
	Operand() = default;
	virtual ~Operand(void) {};
	virtual bool IsTempOperand() { return false; }
	virtual std::string ToString() = 0;
	OperandType get_operand_type() { return operand_type; }
};

class SymbolOperand : public Operand {
private:
	std::string operand_name;
	bool is_temp_operand;
public:
	SymbolOperand(std::string name) {
		operand_type = SymOperandType;
		operand_name = name;
		is_temp_operand = false;
	}
	SymbolOperand(std::string name, bool is_temp) {
		operand_type = SymOperandType;
		operand_name = name;
		is_temp_operand = is_temp;
	}
	~SymbolOperand(void) {};
	std::string get_operand_name() { return operand_name; }
	bool IsTempOperand() override { return is_temp_operand; }
	std::string ToString() override { return operand_name; }
};

class ImmeOperand : public Operand {
private:
	int operand_value;
public:
	ImmeOperand(int value) {
		operand_type = ImmOperandType;
		operand_value = value;
	}
	~ImmeOperand(void) {};
	int get_operand_value() { return operand_value; }
	std::string ToString() override { return std::to_string(operand_value); }
};

class StringOperand : public Operand {
private:
	std::string operand_name;
	std::string operand_value;
public:
	StringOperand(std::string name, std::string value) {
		operand_type = StrOperandType;
		operand_name = name;
		operand_value = value;
	}
	~StringOperand(void) {};
	std::string get_operand_name() { return operand_name; }
	std::string get_operand_value() { return operand_value; }
	std::string ToString() override { return "\"" + operand_value + "\"(" + operand_name + ")"; }
};

class LabelOperand : public Operand {
private:
	std::string operand_name;
public:
	LabelOperand(std::string name) {
		operand_type = LabOperandType;
		operand_name = name;
	}
	~LabelOperand(void) {};
	std::string get_operand_name() { return operand_name; }
	std::string ToString() override { return operand_name; }
};

class Quaternion {
private:
	OperatorType op = UNKNOWN_OPERATOR_TYPE;
	Operand *dest_ptr = nullptr, *loperand_ptr = nullptr, *roperand_ptr = nullptr;
public:
	Quaternion() = default;
	Quaternion(OperatorType op) {
		this->op = op;
	}
	Quaternion(OperatorType op, Operand* dest_ptr) {
		this->op = op;
		this->dest_ptr = dest_ptr;
	}
	Quaternion(OperatorType op, Operand* dest_ptr, Operand* loperand_ptr) {
		this->op = op;
		this->dest_ptr = dest_ptr;
		this->loperand_ptr = loperand_ptr;
	}
	Quaternion(OperatorType op, Operand* dest_ptr, Operand* loperand_ptr, Operand* roperand_ptr) {
		this->op = op;
		this->dest_ptr = dest_ptr;
		this->loperand_ptr = loperand_ptr;
		this->roperand_ptr = roperand_ptr;
	}
	~Quaternion(void) {};
	OperatorType get_op() { return op; }
	Operand* get_dest_ptr() { return dest_ptr; }
	Operand* get_loperand_ptr() { return loperand_ptr; }
	Operand* get_roperand_ptr() { return roperand_ptr; }
	std::string ToString() {
		std::string to_string = operator_map.at(op) + "\t";
		if (dest_ptr != nullptr) {
			to_string.append(dest_ptr->ToString());
		}
		if (loperand_ptr != nullptr) {
			to_string.append(", " + loperand_ptr->ToString());
		}
		if (roperand_ptr != nullptr) {
			to_string.append(", " + roperand_ptr->ToString());
		}
		return to_string;
	}
};

namespace ir {
	// 判断该操作符是不是标识符操作数
	bool IsVarSymbolOperand(Operand* operand_ptr);

	SymbolOperand* GetSymbolOperandPtr(std::shared_ptr<Symbol>);

	std::shared_ptr<std::vector<Quaternion>> GetQuatVectorPtr();

	void AddQuaternionBack(Quaternion);

	void AddQuaternionHead(Quaternion);

	std::string ProdStringName();

	Operand* ProdTempOperandPtr();

	void OutputQuatVector();

	void IrOptim();
}