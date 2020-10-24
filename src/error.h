#pragma once
#include <string>

const bool ERROR_TYPE_OUTPUT = true;
const bool ERROR_DESCRIPTION_OUTPUT = false;

enum ErrorType {
	InvalidSymbol,				// a �Ƿ����Ż򲻷��ϴʷ�
	NameRedefine,				// b �����ض���
	NameNotDefine,				// c δ���������
	ArgsNumNotMatch,			// d ��������������ƥ��
	ArgsTypeNotMatch,			// e �����������Ͳ�ƥ��
	ConditionJudgeInvalidType,	// f �����ж��г��ֲ��Ϸ�������
	NoRetFuncReturn,			// g �޷���ֵ�ĺ������ڲ�ƥ���return���
	RetFuncMissReturn,			// h �з���ֵ�ĺ���ȱ��return������ڲ�ƥ���return���
	ArrIndexNotInt,				// i ����Ԫ�ص��±�ֻ�������ͱ��ʽ
	ModifyConst,				// j ���ܸı䳣����ֵ
	SemicnMiss,					// k ӦΪ�ֺ�
	RparentMiss,				// l ӦΪ��С����')'
	RbrackMiss,					// m ӦΪ��������']'
	ArrInitNotMatch,			// n �����ʼ��������ƥ��
	VarTypeNotMatch,			// o <����>���Ͳ�һ��
	MissDefault,				// p ȱ��ȱʡ���
	OtherError,
};

namespace error {
	std::string GetErrorTypeNum(ErrorType error_type);
	void OutputError(int error_line, ErrorType error_type);
}