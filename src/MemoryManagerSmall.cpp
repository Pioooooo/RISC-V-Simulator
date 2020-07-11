//
// Created by Pioooooo on 2020/7/11.
//

#include "MemoryManagerSmall.h"

MemoryManagerSmall::MemoryManagerSmall()
{
	memset(memory, 0, sizeof(__uint8_t) * MEMORY_SIZE);
}

void MemoryManagerSmall::setByte(__uint32_t addr, __uint8_t val)
{
	memcpy(memory + addr, &val, sizeof(__uint8_t));
}

void MemoryManagerSmall::setShort(__uint32_t addr, __uint16_t val)
{
	memcpy(memory + addr, &val, sizeof(__uint16_t));
}

void MemoryManagerSmall::setInt(__uint32_t addr, __uint32_t val)
{
	memcpy(memory + addr, &val, sizeof(__uint32_t));
}

__uint8_t MemoryManagerSmall::getByte(__uint32_t addr)
{
	__uint8_t ret;
	memcpy(&ret, memory + addr, sizeof(__uint8_t));
}

__uint16_t MemoryManagerSmall::getShort(__uint32_t addr)
{
	__uint16_t ret;
	memcpy(&ret, memory + addr, sizeof(__uint16_t));
}

__uint32_t MemoryManagerSmall::getInt(__uint32_t addr)
{
	__uint32_t ret;
	memcpy(&ret, memory + addr, sizeof(__uint32_t));
}
