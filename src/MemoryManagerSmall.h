//
// Created by Pioooooo on 2020/7/11.
//

#ifndef CODE_MEMORYMANAGERSMALL_H
#define CODE_MEMORYMANAGERSMALL_H

#include <cstring>
#include "MemoryManager.h"

#define MEMORY_SIZE 0x20000

class MemoryManagerSmall: public MemoryManager
{
	__uint8_t memory[MEMORY_SIZE];
public:
	MemoryManagerSmall();
	
	~MemoryManagerSmall() = default;
	
	void setByte(__uint32_t addr, __uint8_t val) override;
	
	void setShort(__uint32_t addr, __uint16_t val) override;
	
	void setInt(__uint32_t addr, __uint32_t val) override;
	
	__uint8_t getByte(__uint32_t addr) override;
	
	__uint16_t getShort(__uint32_t addr) override;
	
	__uint32_t getInt(__uint32_t addr) override;
	
};


#endif //CODE_MEMORYMANAGERSMALL_H
