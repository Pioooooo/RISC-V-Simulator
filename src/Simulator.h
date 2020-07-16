//
// Created by Pioooooo on 2020/7/6.
//

#ifndef RISC_V_SIMULATOR_H
#define RISC_V_SIMULATOR_H

#include <cstdio>

#include "MemoryManagerLarge.h"
#include "MemoryManagerSmall.h"
#include "Debug.h"
#include "BranchPredictor.h"

namespace RISCV
{
	typedef __int32_t RegId;
	
	extern const char *REGNAME[], *INSTNAME[];
	
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
		LBU = 13,
		LHU = 14,
		SB = 15,
		SH = 16,
		SW = 17,
		ADDI = 18,
		SLTI = 19,
		SLTIU = 20,
		XORI = 21,
		ORI = 22,
		ANDI = 23,
		SLLI = 24,
		SRLI = 25,
		SRAI = 26,
		ADD = 27,
		SUB = 28,
		SLL = 29,
		SLT = 30,
		SLTU = 31,
		XOR = 32,
		SRL = 33,
		SRA = 34,
		OR = 35,
		AND = 36,
		UNKNOWN = -1
	};
	
	enum Stat
	{
		MEM_BYTE = 0x01,
		MEM_HALF = 0x02,
		MEM_WORD = 0x03,
		MEM_LENGTH = 0x03,
		MEM_SIGN = 0x04,
		WRITE_REG = 0X08,
		WRITE_MEM = 0x10,
		READ_MEM = 0x20,
		TIME_ONE = 0x80,
		TIME_TWO = 0x40,
		TIME_THREE = 0x00,
		TIME_DELTA = 0x40,
		TIME_LENGTH = 0xC0
	};
}

class Simulator
{
	__uint32_t pc, cycle{};
	__int32_t *reg;
	
	MemoryManager *memory;
	BranchPredictor *predictor;
	
	struct datIF
	{
		__uint32_t inst, pc;
		bool busy;
	} regIF;
	
	struct datID
	{
		RISCV::RegId rd;
		RISCV::Inst inst;
		__int32_t rs1, rs2, imm, pc;
		bool busy, pred;
	} regID;
	
	struct datEX
	{
		RISCV::RegId rd;
		__uint8_t stat;
		__int32_t output, val;
		bool busy;
	} regEX;
	
	struct datMEM
	{
		RISCV::RegId rd;
		__int32_t output;
		__uint8_t stat;
		bool busy;
	} regMEM;
	
	void IF();
	
	void ID();
	
	void EX();
	
	void MEM();
	
	void WB();

public:
	explicit Simulator(FILE *data);
	
	~Simulator();
	
	void run();
};


#endif //RISC_V_SIMULATOR_H
