//
// Created by Pioooooo on 2020/7/6.
//

#include "Simulator.h"
#include <string>

using namespace RISCV;

Simulator::Simulator(const char *fileName): reg(new __int32_t[32]), pc(0), memory(new MemoryManager()), regIF(), regID(), regEX(), regMEM(), regWB()
{
	FILE *data(fopen(fileName, "r"));
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

void Simulator::run()
{
	while(true)
	{
		IF();
		if(regIF.inst == 0x00f00513)
		{
			printf("%d", (__uint32_t)reg[10] & 0xffu);
			break;
		}
		ID();
		EX();
		MEM();
		WB();
	}
}

void Simulator::IF()
{
	regIF.inst = memory->getInt(pc);
	pc += 4;
}

void Simulator::ID()
{
	const __uint32_t &inst = regIF.inst;
	Inst &instType = regID.inst = Inst::UNKNOWN;
	RegId &dest = regID.rd = -1;
	__int32_t &op1 = regID.op1 = 0, &op2 = regID.op2 = 0, &offset = regID.offset;


	__uint32_t opcode = inst & 0x7fu;
	__uint32_t funct3 = (inst >> 12u) & 0x7u;
	__uint32_t funct7 = (inst >> 25u) & 0x7fu;
	RegId rd = (inst >> 7u) & 0x1fu;
	RegId rs1 = (inst >> 15u) & 0x1fu;
	RegId rs2 = (inst >> 20u) & 0x1fu;
	__int32_t immI = ((__int32_t)inst >> 20) & 0xfff;
	__int32_t immS = __int32_t(((inst >> 20u) & 0xfe0u) | ((inst >> 7u) & 0x1fu)) << 20 >> 20;
	__int32_t immB = __int32_t(((inst >> 19u) & 0x1000u) | ((inst >> 20u) & 0x7e0u) | ((inst >> 7u) & 0x1eu) | ((inst << 4u) & 0x800u)) << 20 >> 20;
	__int32_t immU = inst & 0xfffff000u;
	__int32_t immJ = __int32_t(((inst >> 11u) & 0x80000u) | ((inst >> 20u) & 0xffeu) | ((inst >> 9u) & 0x800u) | (inst & 0xff000u)) << 11 >> 11;

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
			if(funct7 == 0x00)
			{
				instType = SRLI;
				op2 = rs2;
			}
			else if(funct7 == 0x20)
			{
				instType = SRAI;
				op2 = rs2;
			}
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
			else if(funct7 == 0x10)
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
			else if(funct7 == 0x10)
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
}

void Simulator::EX()
{
	Inst &inst = regID.inst;
	__int32_t &op1 = regID.op1, &op2 = regID.op2, &offset = regID.offset, &output = regEX.output = 0, &val = regEX.val;
	__uint8_t &stat = regEX.stat = 0;
	__uint32_t &pcEX = regEX.pc = pc;
	regEX.rd = regID.rd;
	regEX.inst = regID.inst;
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
		stat |= Stat::WRITE_REG | Stat::BRANCH;
		output = pc + 4;
		pcEX += op1;
		break;
	case JALR:
		stat |= Stat::WRITE_REG | Stat::BRANCH;
		output = (op1 + op2) & ~(__uint32_t)1;
		break;
	case BEQ:
		if(op1 == op2)
		{
			stat |= Stat::BRANCH;
			pcEX = pc + offset;
		}
		break;
	case BNE:
		if(op1 != op2)
		{
			stat |= Stat::BRANCH;
			pcEX = pc + offset;
		}
		break;
	case BLT:
		if(op1 < op2)
		{
			stat |= Stat::BRANCH;
			pcEX = pc + offset;
		}
		break;
	case BGE:
		if(op1 >= op2)
		{
			stat |= Stat::BRANCH;
			pcEX = pc + offset;
		}
		break;
	case BLTU:
		if((__uint32_t)op1 < (__uint32_t)op2)
		{
			stat |= Stat::BRANCH;
			pcEX = pc + offset;
		}
		break;
	case BGEU:
		if((__uint32_t)op1 >= (__uint32_t)op2)
		{
			stat |= Stat::BRANCH;
			pcEX = pc + offset;
		}
		break;
	case LB:
		stat |= Stat::WRITE_REG | Stat::READ_MEM | Stat::MEM_BYTE | Stat::MEM_SIGN;
		output = op1 + offset;
		break;
	case LH:
		stat |= Stat::WRITE_REG | Stat::READ_MEM | Stat::MEM_HALF | Stat::MEM_SIGN;
		output = op1 + offset;
		break;
	case LW:
		stat |= Stat::WRITE_REG | Stat::READ_MEM | Stat::MEM_WORD | Stat::MEM_SIGN;
		output = op1 + offset;
		break;
	case LBU:
		stat |= Stat::WRITE_REG | Stat::READ_MEM | Stat::MEM_BYTE;
		output = op1 + offset;
		break;
	case LHU:
		stat |= Stat::WRITE_REG | Stat::READ_MEM | Stat::MEM_HALF;
		output = op1 + offset;
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
	__int32_t &output = regEX.output, &val = regEX.val, &out = regMEM.output;
	regMEM.rd = regEX.rd;
	regMEM.inst = regEX.inst;
	regMEM.pc = regEX.pc;
	regMEM.stat = regEX.stat;
	if(stat | Stat::WRITE_MEM)
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
	if(stat | Stat::READ_MEM)
		switch(stat & Stat::MEM_LENGTH)
		{
		case Stat::MEM_BYTE:
			if(stat | Stat::MEM_SIGN)
				out = (__int32_t)memory->getByte(output);
			else
				out = (__uint32_t)memory->getByte(output);
			break;
		case Stat::MEM_HALF:
			if(stat | Stat::MEM_SIGN)
				out = (__int32_t)memory->getShort(output);
			else
				out = (__uint32_t)memory->getShort(output);
			break;
		case Stat::MEM_WORD:
			if(stat | Stat::MEM_SIGN)
				out = (__int32_t)memory->getInt(output);
			else
				out = (__uint32_t)memory->getInt(output);
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
	if(stat | Stat::WRITE_REG && rd)
		reg[rd] = output;
	pc = regMEM.pc;
}
