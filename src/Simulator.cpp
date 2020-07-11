//
// Created by Pioooooo on 2020/7/6.
//

#include "Simulator.h"
#include "Debug.h"
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
		reg(new __int32_t[32]), pc(0), memory(new MemoryManager()), regIF(), regID(), regEX(), regMEM(), regWB()
{
	char buf[20];
	int cur = 0;
	while(fscanf(data, " \n%s", buf) != EOF)
	{
		if(buf[0] == '@')
			cur = stoi(buf + 1);
		else
			memory->setByte(cur++, stoi(buf));
	}
	fclose(data);
}

Simulator::~Simulator()
{
	delete[] reg;
	delete memory;
}

void Simulator::run()
{
	cycle = 0;
	while(true)
	{
		IF();
		if(regIF.inst == 0x0ff00513)
		{
			printf("%d", (__uint32_t)reg[10] & 0xffu);
			break;
		}
		ID();
		EX();
		MEM();
		WB();
		cycle++;
	}
}

void Simulator::IF()
{
	regIF.inst = memory->getInt(pc);
	regIF.pc = pc + 4;
	//DEBUG("Inst code: 0x%.8X", regIF.inst);
}

void Simulator::ID()
{
	const __uint32_t &inst = regIF.inst;
	Inst &instType = regID.inst = Inst::UNKNOWN;
	RegId &dest = regID.rd = -1;
	__int32_t &op1 = regID.op1 = 0, &op2 = regID.op2 = 0, &offset = regID.offset = 0;
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
		op1 = reg[rs1];
		op2 = immI;
		dest = rd;
		switch(funct3)
		{
		case 0b000:
			instType = ADDI;
			break;
		case 0b001:
			instType = SLLI;
			op2 = rs2;
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
			op2 = rs2;
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
		op1 = immU;
		dest = rd;
		instType = LUI;
		break;
	case OP_AUIPC:
		op1 = immU;
		dest = rd;
		instType = AUIPC;
		break;
	case OP_OP:
		op1 = reg[rs1];
		op2 = reg[rs2];
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
		op1 = immJ;
		dest = rd;
		instType = JAL;
		break;
	case OP_JALR:
		op1 = reg[rs1];
		op2 = immI;
		dest = rd;
		instType = JALR;
		break;
	case OP_BRANCH:
		op1 = reg[rs1];
		op2 = reg[rs2];
		offset = immB;
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
		break;
	case OP_LOAD:
		op1 = reg[rs1];
		op2 = immI;
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
		op1 = reg[rs1];
		op2 = reg[rs2];
		offset = immS;
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
	if(cycle <= 205)
		DEBUG("cycle:%4d pc:0x%.8X dat:0x%.8X inst:%s op1:0x%.8X op2:0x%.8X offset:0x%.8X dest:%s rs1:%s rs2:%s", cycle, pc, inst, INSTNAME[instType], op1, op2, offset, REGNAME[rd], REGNAME[rs1], REGNAME[rs2]);
}

void Simulator::EX()
{
	Inst &inst = regID.inst;
	__int32_t &op1 = regID.op1, &op2 = regID.op2, &offset = regID.offset, &output = regEX.output = 0, &val = regEX.val, &pc = regEX.pc = regID.pc;;
	__uint8_t &stat = regEX.stat = 0;
	regEX.rd = regID.rd;
	switch(inst)
	{
	case LUI:
		stat |= Stat::WRITE_REG;
		output = op1;
		break;
	case AUIPC:
		stat |= Stat::WRITE_REG;
		output = pc + op1;
		break;
	case JAL:
		stat |= Stat::WRITE_REG;
		output = pc;
		pc += op1 - 4;
		break;
	case JALR:
		stat |= Stat::WRITE_REG;
		output = pc;
		pc = (op1 + op2) & (~(__uint32_t)1);
		break;
	case BEQ:
		if(op1 == op2)
		{
			stat |= Stat::BRANCH;
			pc += offset - 4;
		}
		break;
	case BNE:
		if(op1 != op2)
		{
			stat |= Stat::BRANCH;
			pc += offset - 4;
		}
		break;
	case BLT:
		if(op1 < op2)
		{
			stat |= Stat::BRANCH;
			pc += offset - 4;
		}
		break;
	case BGE:
		if(op1 >= op2)
		{
			stat |= Stat::BRANCH;
			pc += offset - 4;
		}
		break;
	case BLTU:
		if((__uint32_t)op1 < (__uint32_t)op2)
		{
			stat |= Stat::BRANCH;
			pc += offset - 4;
		}
		break;
	case BGEU:
		if((__uint32_t)op1 >= (__uint32_t)op2)
		{
			stat |= Stat::BRANCH;
			pc += offset - 4;
		}
		break;
	case LB:
		stat |= Stat::WRITE_REG | Stat::READ_MEM | Stat::MEM_BYTE | Stat::MEM_SIGN;
		output = op1 + op2;
		break;
	case LH:
		stat |= Stat::WRITE_REG | Stat::READ_MEM | Stat::MEM_HALF | Stat::MEM_SIGN;
		output = op1 + op2;
		break;
	case LW:
		stat |= Stat::WRITE_REG | Stat::READ_MEM | Stat::MEM_WORD | Stat::MEM_SIGN;
		output = op1 + op2;
		break;
	case LBU:
		stat |= Stat::WRITE_REG | Stat::READ_MEM | Stat::MEM_BYTE;
		output = op1 + op2;
		break;
	case LHU:
		stat |= Stat::WRITE_REG | Stat::READ_MEM | Stat::MEM_HALF;
		output = op1 + op2;
		break;
	case SB:
		stat |= Stat::WRITE_MEM | Stat::MEM_BYTE;
		output = op1 + offset;
		val = op2;
		break;
	case SH:
		stat |= Stat::WRITE_MEM | Stat::MEM_HALF;
		output = op1 + offset;
		val = op2;
		break;
	case SW:
		stat |= Stat::WRITE_MEM | Stat::MEM_WORD;
		output = op1 + offset;
		val = op2;
		break;
	case ADD:
	case ADDI:
		stat |= Stat::WRITE_REG;
		output = op1 + op2;
		break;
	case SLT:
	case SLTI:
		stat |= Stat::WRITE_REG;
		output = op1 < op2 ? 1 : 0;
		break;
	case SLTU:
	case SLTIU:
		stat |= Stat::WRITE_REG;
		output = (__uint32_t)op1 < (__uint32_t)op2 ? 1 : 0;
		break;
	case XOR:
	case XORI:
		stat |= Stat::WRITE_REG;
		output = op1 ^ op2;
		break;
	case OR:
	case ORI:
		stat |= Stat::WRITE_REG;
		output = op1 | op2;
		break;
	case AND:
	case ANDI:
		stat |= Stat::WRITE_REG;
		output = op1 & op2;
		break;
	case SLL:
	case SLLI:
		stat |= Stat::WRITE_REG;
		output = op1 << op2;
		break;
	case SRL:
	case SRLI:
		stat |= Stat::WRITE_REG;
		output = (__uint32_t)op1 >> op2;
		break;
	case SRA:
	case SRAI:
		stat |= Stat::WRITE_REG;
		output = op1 >> op2;
		break;
	case SUB:
		stat |= Stat::WRITE_REG;
		output = op1 - op2;
		break;
	default:
		break;
	}
}

void Simulator::MEM()
{
	__uint8_t &stat = regEX.stat;
	__int32_t &output = regMEM.output = regEX.output, &val = regEX.val;
	regMEM.rd = regEX.rd;
	regMEM.stat = regEX.stat;
	regMEM.pc = regEX.pc;
	if(stat & Stat::WRITE_MEM)
		switch(stat & Stat::MEM_LENGTH)
		{
		case Stat::MEM_BYTE:
			memory->setByte(output, val);
			break;
		case Stat::MEM_HALF:
			memory->setShort(output, val);
			break;
		case Stat::MEM_WORD:
			memory->setInt(output, val);
			break;
		default:
			break;
		}
	if(stat & Stat::READ_MEM)
		switch(stat & Stat::MEM_LENGTH)
		{
		case Stat::MEM_BYTE:
			if(stat & Stat::MEM_SIGN)
				output = (__int32_t)memory->getByte(output);
			else
				output = (__uint32_t)memory->getByte(output);
			break;
		case Stat::MEM_HALF:
			if(stat & Stat::MEM_SIGN)
				output = (__int32_t)memory->getShort(output);
			else
				output = (__uint32_t)memory->getShort(output);
			break;
		case Stat::MEM_WORD:
			if(stat & Stat::MEM_SIGN)
				output = (__int32_t)memory->getInt(output);
			else
				output = (__uint32_t)memory->getInt(output);
			break;
		default:
			break;
		}
}

void Simulator::WB()
{
	RegId &rd = regMEM.rd;
	__uint8_t &stat = regMEM.stat;
	__int32_t &output = regMEM.output;
	if(stat & Stat::WRITE_REG && rd)
		reg[rd] = output;
	pc = regMEM.pc;
}

__uint32_t Simulator::stoi(char *str)
{
	__uint32_t ret = 0;
	while(true)
	{
		if('0' <= *str && *str <= '9')
		{
			ret <<= 4u;
			ret += *str - '0';
		}
		else if('A' <= *str && *str <= 'F')
		{
			ret <<= 4u;
			ret += *str - 'A' + 10;
		}
		else
			break;
		str++;
	}
	return ret;
}
