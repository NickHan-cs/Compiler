#pragma once
#include <set>
#include <vector>
#include <memory>
#include "ir.h"

class BasicBlock {
public:
	int begin_ir_idx;
	int end_ir_idx;
	std::set<std::string> in_set;
	std::set<std::string> use_set;
	std::set<std::string> out_set;
	std::set<std::string> def_set;
	std::vector<BasicBlock*> next_basic_block_ptrs;

	BasicBlock(int begin_ir_idx, int end_ir_idx) {
		this->begin_ir_idx = begin_ir_idx;
		this->end_ir_idx = end_ir_idx;
	}

	~BasicBlock(void) {};

	void AddNextBasicBlock(BasicBlock* next_basic_block_ptr) {
		next_basic_block_ptrs.push_back(next_basic_block_ptr);
	}
};

// ���ֻ�����
std::vector<BasicBlock*> DivideBasicBlocks(std::shared_ptr<std::vector<Quaternion>> ir_vector_ptr);

// ������ͼ
void GenFlowGraph(std::shared_ptr<std::vector<Quaternion>> const ir_vector_ptr, const std::vector<BasicBlock*> basic_block_ptrs);

// ����ÿ���������use���Ϻ�def����
void GenUseDefSet(std::shared_ptr<std::vector<Quaternion>> const ir_vector_ptr, const std::vector<BasicBlock*> basic_block_ptrs);

// ��Ծ��������
void LiveVariableAnalysis(std::shared_ptr<std::vector<Quaternion>> const ir_vector_ptr, const std::vector<BasicBlock*> basic_block_ptrs);
