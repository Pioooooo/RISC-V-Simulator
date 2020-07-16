//
// Created by Pioooooo on 2020/7/6.
//

#ifndef RISC_V_MEMORYMANAGER_H
#define RISC_V_MEMORYMANAGER_H

#include <cstdio>

class MemoryManager
{
public:
	MemoryManager() = default;
	
	virtual ~MemoryManager() = default;
	
	virtual void setByte(__uint32_t addr, __uint8_t val) = 0;
	
	virtual void setShort(__uint32_t addr, __uint16_t val) = 0;
	
	virtual void setInt(__uint32_t addr, __uint32_t val) = 0;
	
	virtual __uint8_t getByte(__uint32_t addr) = 0;
	
	virtual __uint16_t getShort(__uint32_t addr) = 0;
	
	virtual __uint32_t getInt(__uint32_t addr) = 0;
};


#endif //RISC_V_MEMORYMANAGER_H
