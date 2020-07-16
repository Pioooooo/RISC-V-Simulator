//
// Created by Pioooooo on 2020/7/14.
//

#ifndef CODE_BRANCHPREDICTOR_H
#define CODE_BRANCHPREDICTOR_H

#include <cstdio>
#include <cstring>

#define PRED_BUF_SIZE 1024

class BranchPredictor
{
	enum State
	{
		STRONG_NOT_TAKEN = 0,
		WEAK_NOT_TAKEN = 1,
		WEAK_TAKEN = 2,
		STRONG_TAKEN = 3,
	};
	__uint8_t predBuf[PRED_BUF_SIZE]{};
	__uint32_t total, success;
	
	static __uint8_t hash(__uint32_t x);

public:
	BranchPredictor();
	
	~BranchPredictor() = default;
	
	bool predict(__uint32_t pc);
	
	void update(__uint32_t pc, bool branch);
	
	void print(FILE *file = stderr);
};

#endif //CODE_BRANCHPREDICTOR_H
