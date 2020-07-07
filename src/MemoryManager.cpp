//
// Created by Pioooooo on 2020/7/6.
//

#include <cstring>
#include "MemoryManager.h"

MemoryManager::MemoryManager(): memory(new __uint8_t **[__MAX_FIRST_INDEX])
{
	memset(memory, 0, sizeof(__uint8_t **) * __MAX_FIRST_INDEX);
}

MemoryManager::~MemoryManager()
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

__uint32_t MemoryManager::getFirstIndex(__uint32_t addr)
{
	return (addr >> 22u) & 0x3ffu;
}

__uint32_t MemoryManager::getSecondIndex(__uint32_t addr)
{
	return (addr >> 12u) & 0x3ffu;
}

__uint32_t MemoryManager::getOffset(__uint32_t addr)
{
	return addr & 0xfffu;
}

void MemoryManager::addPage(__uint32_t addr)
{
	__uint32_t i = getFirstIndex(addr);
	__uint32_t j = getSecondIndex(addr);
	if(memory[i] == nullptr)
	{
		memory[i] = new __uint8_t *[__MAX_SECOND_INDEX];
		memset(memory[i], 0, sizeof(__uint8_t *) * __MAX_SECOND_INDEX);
	}
	if(memory[i][j] == nullptr)
	{
		memory[i][j] = new __uint8_t[__PAGE_SIZE];
		memset(memory[i][j], 0, sizeof(__uint8_t) * __PAGE_SIZE);
	}
}

bool MemoryManager::isAddrExist(__uint32_t addr)
{
	__uint32_t i = getFirstIndex(addr);
	__uint32_t j = getSecondIndex(addr);
	return memory[i] && memory[i][j];
}

void MemoryManager::setByte(__uint32_t addr, __uint8_t val)
{
	if(!isAddrExist(addr))
		addPage(addr);
	__uint32_t i = getFirstIndex(addr);
	__uint32_t j = getSecondIndex(addr);
	__uint32_t k = getOffset(addr);
	memory[i][j][k] = val;
}

__uint8_t MemoryManager::getByte(__uint32_t addr)
{
	if(!isAddrExist(addr))
		return 0;
	__uint32_t i = getFirstIndex(addr);
	__uint32_t j = getSecondIndex(addr);
	__uint32_t k = getOffset(addr);
	return memory[i][j][k];
}

__uint32_t MemoryManager::getInt(__uint32_t addr)
{
	return (__uint32_t)(getByte(addr)) + (getByte(addr + 1) << 8u) + (getByte(addr + 2) << 16u) + (getByte(addr + 3) << 24u);
}
