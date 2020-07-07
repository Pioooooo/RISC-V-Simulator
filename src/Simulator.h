//
// Created by Pioooooo on 2020/7/6.
//

#ifndef RISC_V_SIMULATOR_H
#define RISC_V_SIMULATOR_H

#include <cstdio>

#include "MemoryManager.h"

namespace RISCV
{
	typedef __int32_t RegId;

	enum OP
	{
		OP_IMM = 0x13,
		OP_LUI = 0x37,
		OP_AUIPC = 0x17,
		OP_OP = 0x33,
		OP_JAL = 0x6f,
		OP_JALR = 0x67,
		OP_BRANCH = 0x63,
		OP_LOAD = 0x03,
		OP_STORE = 0x23
	};

	enum Inst
	{
		LUI = 0,
		AUIPC = 1,
		JAL = 2,
		JALR = 3,
		BEQ = 4,
		BNE = 5,
		BLT = 6,
		BGE = 7,
		BLTU = 8,
		BGEU = 9,
		LB = 10,
		LH = 11,
		LW = 12,
		LD = 13,
		LBU = 14,
		LHU = 15,
		SB = 16,
		SH = 17,
		SW = 18,
		SD = 19,
		ADDI = 20,
		SLTI = 21,
		SLTIU = 22,
		XORI = 23,
		ORI = 24,
		ANDI = 25,
		SLLI = 26,
		SRLI = 27,
		SRAI = 28,
		ADD = 29,
		SUB = 30,
		SLL = 31,
		SLT = 32,
		SLTU = 33,
		XOR = 34,
		SRL = 35,
		SRA = 36,
		OR = 37,
		AND = 38,
		UNKNOWN = -1;
	};


}

class Simulator
{
	__uint32_t *reg, pc;

	MemoryManager *memory;

	static __uint32_t stoi(char *str);

	struct datIF
	{
		__uint32_t inst;
	} regIF;

	struct datID
	{
		RISCV::RegId rd;
		RISCV::Inst inst;
		__int32_t op1, op2, offset;
	} regID;

	struct datEX
	{

	} regEX;

	struct datMEM
	{

	} regMEM;

	struct datWB
	{

	} regWB;

	void IF();

	void ID();

	void EX();

	void MEM();

	void WB();

public:
	explicit Simulator(const char *fileName);

	~Simulator();

	void run();
};


#endif //RISC_V_SIMULATOR_H
