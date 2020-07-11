//
// Created by Pioooooo on 2020/7/11.
//

#include "MemoryManagerLarge.h"

MemoryManagerLarge::MemoryManagerLarge(): memory(new __uint8_t **[__MAX_FIRST_INDEX])
{
	memset(memory, 0, sizeof(__uint8_t * *) * __MAX_FIRST_INDEX);
}

MemoryManagerLarge::~MemoryManagerLarge()
{
	for(int i = 0; i < __MAX_FIRST_INDEX; i++)
		if(memory[i])
		{
			for(int j = 0; j < __MAX_SECOND_INDEX; j++)
				if(memory[i][j])
					delete[] memory[i][j];
			delete[] memory[i];
		}
	delete[] memory;
	memory = nullptr;
}

__uint32_t MemoryManagerLarge::getFirstIndex(__uint32_t addr)
{
	return (addr >> 22u) & 0x3ffu;
}

__uint32_t MemoryManagerLarge::getSecondIndex(__uint32_t addr)
{
	return (addr >> 12u) & 0x3ffu;
}

__uint32_t MemoryManagerLarge::getOffset(__uint32_t addr)
{
	return addr & 0xfffu;
}

void MemoryManagerLarge::addPage(__uint32_t addr)
{
	__uint32_t i = getFirstIndex(addr);
	__uint32_t j = getSecondIndex(addr);
	if(memory[i] == nullptr)
	{
		memory[i] = new __uint8_t *[__MAX_SECOND_INDEX];
		memset(memory[i], 0, sizeof(__uint8_t * ) * __MAX_SECOND_INDEX);
	}
	if(memory[i][j] == nullptr)
	{
		memory[i][j] = new __uint8_t[__PAGE_SIZE];
		memset(memory[i][j], 0, sizeof(__uint8_t) * __PAGE_SIZE);
	}
}

bool MemoryManagerLarge::isAddrExist(__uint32_t addr)
{
	__uint32_t i = getFirstIndex(addr);
	__uint32_t j = getSecondIndex(addr);
	return memory[i] && memory[i][j];
}

void MemoryManagerLarge::setByte(__uint32_t addr, __uint8_t val)
{
	if(!isAddrExist(addr))
		addPage(addr);
	__uint32_t i = getFirstIndex(addr);
	__uint32_t j = getSecondIndex(addr);
	__uint32_t k = getOffset(addr);
	memory[i][j][k] = val;
}

void MemoryManagerLarge::setShort(__uint32_t addr, __uint16_t val)
{
	setByte(addr, val);
	setByte(addr + 1, val >> 8u);
}

void MemoryManagerLarge::setInt(__uint32_t addr, __uint32_t val)
{
	setByte(addr, val);
	setByte(addr + 1, val >> 8u);
	setByte(addr + 2, val >> 16u);
	setByte(addr + 3, val >> 24u);
}

__uint8_t MemoryManagerLarge::getByte(__uint32_t addr)
{
	if(!isAddrExist(addr))
		return 0;
	__uint32_t i = getFirstIndex(addr);
	__uint32_t j = getSecondIndex(addr);
	__uint32_t k = getOffset(addr);
	return memory[i][j][k];
}

__uint16_t MemoryManagerLarge::getShort(__uint32_t addr)
{
	return getByte(addr) + ((__uint16_t)getByte(addr + 1) << 8u);
}

__uint32_t MemoryManagerLarge::getInt(__uint32_t addr)
{
	return getByte(addr) + ((__uint32_t)getByte(addr + 1) << 8u) + ((__uint32_t)getByte(addr + 2) << 16u) + ((__uint32_t)getByte(addr + 3) << 24u);
}

