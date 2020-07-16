//
// Created by Pioooooo on 2020/7/6.
//

#include "Simulator.h"
#include <string>

namespace RISCV
{
	const char *REGNAME[32] = {"zero",  // x0
							   "ra	",  // x1
							   "sp	",  // x2
							   "gp	",  // x3
							   "tp	",  // x4
							   "t0	",  // x5
							   "t1	",  // x6
							   "t2	",  // x7
							   "s0	",  // x8
							   "s1	",  // x9
							   "a0	",  // x10
							   "a1	",  // x11
							   "a2	",  // x12
							   "a3	",  // x13
							   "a4	",  // x14
							   "a5	",  // x15
							   "a6	",  // x16
							   "a7	",  // x17
							   "s2	",  // x18
							   "s3	",  // x19
							   "s4  ",  // x20
							   "s5  ",  // x21
							   "s6  ",  // x22
							   "s7  ",  // x23
							   "s8  ",  // x24
							   "s9  ",  // x25
							   "s10 ",  // x26
							   "s11 ",  // x27
							   "t3  ",  // x28
							   "t4  ",  // x29
							   "t5  ",  // x30
							   "t6  ",  // x31
	};
	const char *INSTNAME[] = {"lui  ", "auipc", "jal  ", "jalr ", "beq  ", "bne  ", "blt  ", "bge  ", "bltu ", "bgeu ", "lb   ", "lh   ",//
							  "lw   ", "lbu  ", "lhu  ", "sb   ", "sh   ", "sw   ", "addi ", "slti ", "sltiu", "xori ", "ori  ", "andi ",//
							  "slli ", "srli ", "srai ", "add  ", "sub  ", "sll  ", "slt  ", "sltu ", "xor  ", "srl  ", "sra  ", "or   ", "and  "};
}

using namespace RISCV;

Simulator::Simulator(FILE *data):
		reg(new __int32_t[32]), pc(0), memory(new MemoryManagerSmall()), predictor(new BranchPredictor()), regIF(), regID(), regEX(), regMEM(), cycle(0)
{
	char buf[20];
	__int32_t cur = 0;
	while(fscanf(data, " \n%s", buf) != EOF)
	{
		if(buf[0] == '@')
			cur = strtol(buf + 1, nullptr, 16);
		else
			memory->setByte(cur++, strtol(buf, nullptr, 16));
	}
	fclose(data);
}

Simulator::~Simulator()
{
	delete[] reg;
	delete memory;
	delete predictor;
}

void Simulator::run()
{
	cycle = 0;
	while(regIF.inst != 0xFF00513 || regIF.busy || regID.busy || regEX.busy || regMEM.busy)
	{
		WB();
		MEM();
		EX();
		ID();
		IF();
		DEBUG("cycle:%4d pc:0x%.8X dat:0x%.8X inst:%s rs1:0x%.8X rs2:0x%.8X imm:0x%.8X dest:%s rs1:%s rs2:%s", cycle, pc - 4, regIF.inst, INSTNAME[regID.inst], regID.rs1, regID.rs2, regID.imm, REGNAME[regID.rd], REGNAME[regID.rs1], REGNAME[regID.rs2]);
		cycle++;
	}
	printf("%d", (__uint32_t)reg[10] & 0xffu);
}

void Simulator::IF()
{
	if(regIF.busy)
		return;
	regIF.inst = memory->getInt(pc);
	if(regIF.inst == 0x0ff00513)
		return;
	regIF.pc = pc;
	pc += 4;
	regIF.busy = true;
	//DEBUG("Inst code: 0x%.8X", regIF.inst);
}

void Simulator::ID()
{
	if(!regIF.busy || regID.busy)
		return;
	const __uint32_t &inst = regIF.inst;
	Inst &instType = regID.inst = UNKNOWN;
	RegId &dest = regID.rd = -1;
	__int32_t &op1 = regID.rs1 = 0, &op2 = regID.rs2 = 0, &imm = regID.imm = 0;
	regID.pc = regIF.pc;
	__uint32_t opcode = inst & 0x7fu;
	__uint32_t funct3 = (inst >> 12u) & 0x7u;
	__uint32_t funct7 = (inst >> 25u) & 0x7fu;
	RegId rd = (inst >> 7u) & 0x1fu;
	RegId rs1 = (inst >> 15u) & 0x1fu;
	RegId rs2 = (inst >> 20u) & 0x1fu;
	__int32_t immI = (__int32_t)((inst >> 20u) & 0xfffu) << 20 >> 20;
	__int32_t immS = __int32_t(((inst >> 20u) & 0xfe0u) | ((inst >> 7u) & 0x1fu)) << 20 >> 20;
	__int32_t immB = __int32_t(((inst >> 19u) & 0x1000u) | ((inst >> 20u) & 0x7e0u) | ((inst >> 7u) & 0x1eu) | ((inst << 4u) & 0x800u)) << 20 >> 20;
	__int32_t immU = inst & 0xfffff000u;
	__int32_t immJ = __int32_t(((inst >> 11u) & 0x100000u) | ((inst >> 20u) & 0x7feu) | ((inst >> 9u) & 0x800u) | (inst & 0xff000u)) << 11 >> 11;
	switch(opcode)
	{
	case OP_IMM:
		op1 = rs1;
		imm = immI;
		dest = rd;
		switch(funct3)
		{
		case 0b000:
			instType = ADDI;
			break;
		case 0b001:
			instType = SLLI;
			imm = rs2;
			break;
		case 0b010:
			instType = SLTI;
			break;
		case 0b011:
			instType = SLTIU;
			break;
		case 0b100:
			instType = XORI;
			break;
		case 0b101:
			imm = rs2;
			if(funct7 == 0x00)
				instType = SRLI;
			else if(funct7 == 0x20)
				instType = SRAI;
			break;
		case 0b110:
			instType = ORI;
			break;
		case 0b111:
			instType = ANDI;
			break;
		default:
			break;
		}
		break;
	case OP_LUI:
		imm = immU;
		dest = rd;
		instType = LUI;
		break;
	case OP_AUIPC:
		imm = immU;
		dest = rd;
		instType = AUIPC;
		break;
	case OP_OP:
		op1 = rs1;
		op2 = rs2;
		dest = rd;
		switch(funct3)
		{
		case 0b000:
			if(funct7 == 0x00)
				instType = ADD;
			else if(funct7 == 0x20)
				instType = SUB;
			break;
		case 0b001:
			instType = SLL;
			break;
		case 0b010:
			instType = SLT;
			break;
		case 0b011:
			instType = SLTU;
			break;
		case 0b100:
			instType = XOR;
			break;
		case 0b101:
			if(funct7 == 0x00)
				instType = SRL;
			else if(funct7 == 0x20)
				instType = SRA;
			break;
		case 0b110:
			instType = OR;
			break;
		case 0b111:
			instType = AND;
			break;
		default:
			break;
		}
		break;
	case OP_JAL:
		imm = immJ;
		dest = rd;
		instType = JAL;
		pc = regIF.pc + imm;
		break;
	case OP_JALR:
		op1 = rs1;
		imm = immI;
		dest = rd;
		instType = JALR;
		break;
	case OP_BRANCH:
		op1 = rs1;
		op2 = rs2;
		imm = immB;
		switch(funct3)
		{
		case 0b000:
			instType = BEQ;
			break;
		case 0b001:
			instType = BNE;
			break;
		case 0b100:
			instType = BLT;
			break;
		case 0b101:
			instType = BGE;
			break;
		case 0b110:
			instType = BLTU;
			break;
		case 0b111:
			instType = BGEU;
			break;
		default:
			break;
		}
		regID.pred = predictor->predict(regIF.pc);
		if(regID.pred)
			pc = regID.pc + imm;
		else
			pc = regID.pc + 4;
		break;
	case OP_LOAD:
		op1 = rs1;
		imm = immI;
		dest = rd;
		switch(funct3)
		{
		case 0b000:
			instType = LB;
			break;
		case 0b001:
			instType = LH;
			break;
		case 0b010:
			instType = LW;
			break;
		case 0b100:
			instType = LBU;
			break;
		case 0b101:
			instType = LHU;
			break;
		default:
			break;
		}
		break;
	case OP_STORE:
		op1 = rs1;
		op2 = rs2;
		imm = immS;
		switch(funct3)
		{
		case 0b000:
			instType = SB;
			break;
		case 0b001:
			instType = SH;
			break;
		case 0b010:
			instType = SW;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	if((op1 && regEX.busy && regEX.rd == op1 && regEX.stat & READ_MEM) || (op2 && regEX.busy && regEX.rd == op2 && regEX.stat & READ_MEM))
		return;
	if(op1)
	{
		if(regEX.busy && regEX.rd == op1 && !(regEX.stat & READ_MEM))
			op1 = regEX.output;
		else if(regMEM.busy && regMEM.rd == op1)
			op1 = regMEM.output;
		else
			op1 = reg[op1];
	}
	if(op2)
	{
		if(regEX.busy && regEX.rd == op2 && !(regEX.stat & READ_MEM))
			op2 = regEX.output;
		else if(regMEM.busy && regMEM.rd == op2)
			op2 = regMEM.output;
		else
			op2 = reg[op2];
	}
	if(instType == JALR)
		pc = (op1 + imm) & (~(__uint32_t)1);
	regID.busy = true;
	regIF.busy = false;
}

void Simulator::EX()
{
	if(!regID.busy || regEX.busy)
		return;
	Inst &inst = regID.inst;
	__int32_t &op1 = regID.rs1, &op2 = regID.rs2, &imm = regID.imm, &output = regEX.output = 0, &val = regEX.val;
	__uint8_t &stat = regEX.stat = 0;
	regEX.rd = regID.rd;
	switch(inst)
	{
	case LUI:
		stat |= WRITE_REG;
		output = imm;
		break;
	case AUIPC:
		stat |= WRITE_REG;
		output = regID.pc + imm;
		break;
	case JAL:
		stat |= WRITE_REG;
		output = regID.pc + 4;
		break;
	case JALR:
		stat |= WRITE_REG;
		output = regID.pc + 4;
		break;
	case BEQ:
		predictor->update(regID.pc, op1 == op2);
		if(op1 == op2 && !regID.pred)
		{
			pc = regID.pc + regID.imm;
			regIF.busy = false;
		}
		else if(op1 != op2 && regID.pred)
		{
			pc = regID.pc + 4;
			regIF.busy = false;
		}
		break;
	case BNE:
		if(op1 != op2 && !regID.pred)
		{
			pc = regID.pc + regID.imm;
			regIF.busy = false;
		}
		else if(op1 == op2 && regID.pred)
		{
			pc = regID.pc + 4;
			regIF.busy = false;
		}
		break;
	case BLT:
		if(op1 < op2 && !regID.pred)
		{
			pc = regID.pc + regID.imm;
			regIF.busy = false;
		}
		else if(op1 >= op2 && regID.pred)
		{
			pc = regID.pc + 4;
			regIF.busy = false;
		}
		break;
	case BGE:
		if(op1 >= op2 && !regID.pred)
		{
			pc = regID.pc + regID.imm;
			regIF.busy = false;
		}
		else if(op1 < op2 && regID.pred)
		{
			pc = regID.pc + 4;
			regIF.busy = false;
		}
		break;
	case BLTU:
		if((__uint32_t)op1 < (__uint32_t)op2 && !regID.pred)
		{
			pc = regID.pc + regID.imm;
			regIF.busy = false;
		}
		else if((__uint32_t)op1 >= (__uint32_t)op2 && regID.pred)
		{
			pc = regID.pc + 4;
			regIF.busy = false;
		}
		break;
	case BGEU:
		if((__uint32_t)op1 >= (__uint32_t)op2 && !regID.pred)
		{
			pc = regID.pc + regID.imm;
			regIF.busy = false;
		}
		else if((__uint32_t)op1 < (__uint32_t)op2 && regID.pred)
		{
			pc = regID.pc + 4;
			regIF.busy = false;
		}
		break;
	case LB:
		stat |= WRITE_REG | READ_MEM | MEM_BYTE | MEM_SIGN;
		output = op1 + imm;
		break;
	case LH:
		stat |= WRITE_REG | READ_MEM | MEM_HALF | MEM_SIGN;
		output = op1 + imm;
		break;
	case LW:
		stat |= WRITE_REG | READ_MEM | MEM_WORD | MEM_SIGN;
		output = op1 + imm;
		break;
	case LBU:
		stat |= WRITE_REG | READ_MEM | MEM_BYTE;
		output = op1 + imm;
		break;
	case LHU:
		stat |= WRITE_REG | READ_MEM | MEM_HALF;
		output = op1 + imm;
		break;
	case SB:
		stat |= WRITE_MEM | MEM_BYTE;
		output = op1 + imm;
		val = op2;
		break;
	case SH:
		stat |= WRITE_MEM | MEM_HALF;
		output = op1 + imm;
		val = op2;
		break;
	case SW:
		stat |= WRITE_MEM | MEM_WORD;
		output = op1 + imm;
		val = op2;
		break;
	case ADD:
		stat |= WRITE_REG;
		output = op1 + op2;
		break;
	case ADDI:
		stat |= WRITE_REG;
		output = op1 + imm;
		break;
	case SLT:
		stat |= WRITE_REG;
		output = op1 < op2 ? 1 : 0;
		break;
	case SLTI:
		stat |= WRITE_REG;
		output = op1 < imm ? 1 : 0;
		break;
	case SLTU:
		stat |= WRITE_REG;
		output = (__uint32_t)op1 < (__uint32_t)op2 ? 1 : 0;
		break;
	case SLTIU:
		stat |= WRITE_REG;
		output = (__uint32_t)op1 < (__uint32_t)imm ? 1 : 0;
		break;
	case XOR:
		stat |= WRITE_REG;
		output = op1 ^ op2;
		break;
	case XORI:
		stat |= WRITE_REG;
		output = op1 ^ imm;
		break;
	case OR:
		stat |= WRITE_REG;
		output = op1 | op2;
		break;
	case ORI:
		stat |= WRITE_REG;
		output = op1 | imm;
		break;
	case AND:
		stat |= WRITE_REG;
		output = op1 & op2;
		break;
	case ANDI:
		stat |= WRITE_REG;
		output = op1 & imm;
		break;
	case SLL:
		stat |= WRITE_REG;
		output = op1 << op2;
		break;
	case SLLI:
		stat |= WRITE_REG;
		output = op1 << imm;
		break;
	case SRL:
		stat |= WRITE_REG;
		output = (__uint32_t)op1 >> op2;
		break;
	case SRLI:
		stat |= WRITE_REG;
		output = (__uint32_t)op1 >> imm;
		break;
	case SRA:
		stat |= WRITE_REG;
		output = op1 >> op2;
		break;
	case SRAI:
		stat |= WRITE_REG;
		output = op1 >> imm;
		break;
	case SUB:
		stat |= WRITE_REG;
		output = op1 - op2;
		break;
	default:
		break;
	}
	regEX.stat &= ~TIME_LENGTH;
	regEX.stat |= TIME_ONE;
	regID.busy = false;
	regEX.busy = true;
}

void Simulator::MEM()
{
	if(!regEX.busy || regMEM.busy)
		return;
	__uint8_t &stat = regEX.stat;
	if((stat & WRITE_MEM || stat & READ_MEM) && (stat & TIME_LENGTH) > TIME_THREE)
	{
		stat -= TIME_DELTA;
		return;
	}
	__int32_t &output = regMEM.output = regEX.output, &val = regEX.val;
	regMEM.rd = regEX.rd;
	regMEM.stat = regEX.stat;
	if(stat & WRITE_MEM)
		switch(stat & MEM_LENGTH)
		{
		case MEM_BYTE:
			memory->setByte(output, val);
			break;
		case MEM_HALF:
			memory->setShort(output, val);
			break;
		case MEM_WORD:
			memory->setInt(output, val);
			break;
		default:
			break;
		}
	if(stat & READ_MEM)
		switch(stat & MEM_LENGTH)
		{
		case MEM_BYTE:
			if(stat & MEM_SIGN)
				output = (__int32_t)memory->getByte(output);
			else
				output = (__uint32_t)memory->getByte(output);
			break;
		case MEM_HALF:
			if(stat & MEM_SIGN)
				output = (__int32_t)memory->getShort(output);
			else
				output = (__uint32_t)memory->getShort(output);
			break;
		case MEM_WORD:
			if(stat & MEM_SIGN)
				output = (__int32_t)memory->getInt(output);
			else
				output = (__uint32_t)memory->getInt(output);
			break;
		default:
			break;
		}
	regEX.busy = false;
	regMEM.busy = true;
}

void Simulator::WB()
{
	if(!regMEM.busy)
		return;
	RegId &rd = regMEM.rd;
	__uint8_t &stat = regMEM.stat;
	__int32_t &output = regMEM.output;
	if(stat & WRITE_REG && rd)
		reg[rd] = output;
	regMEM.busy = false;
}

