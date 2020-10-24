#pragma once
#include <string>

const bool ERROR_TYPE_OUTPUT = true;
const bool ERROR_DESCRIPTION_OUTPUT = false;

enum ErrorType {
	InvalidSymbol,				// a 非法符号或不符合词法
	NameRedefine,				// b 名字重定义
	NameNotDefine,				// c 未定义的名字
	ArgsNumNotMatch,			// d 函数参数个数不匹配
	ArgsTypeNotMatch,			// e 函数参数类型不匹配
	ConditionJudgeInvalidType,	// f 条件判断中出现不合法的类型
	NoRetFuncReturn,			// g 无返回值的函数存在不匹配的return语句
	RetFuncMissReturn,			// h 有返回值的函数缺少return语句或存在不匹配的return语句
	ArrIndexNotInt,				// i 数组元素的下标只能是整型表达式
	ModifyConst,				// j 不能改变常量的值
	SemicnMiss,					// k 应为分号
	RparentMiss,				// l 应为右小括号')'
	RbrackMiss,					// m 应为右中括号']'
	ArrInitNotMatch,			// n 数组初始化个数不匹配
	VarTypeNotMatch,			// o <常量>类型不一致
	MissDefault,				// p 缺少缺省语句
	OtherError,
};

namespace error {
	std::string GetErrorTypeNum(ErrorType error_type);
	void OutputError(int error_line, ErrorType error_type);
}