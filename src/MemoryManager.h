//
// Created by Pioooooo on 2020/7/6.
//

#ifndef RISC_V_MEMORYMANAGER_H
#define RISC_V_MEMORYMANAGER_H

#include <cstdio>

#define __MAX_FIRST_INDEX 1024
#define __MAX_SECOND_INDEX 1024
#define __PAGE_SIZE 4096

class MemoryManager
{
	__uint8_t ***memory;

	static __uint32_t getFirstIndex(__uint32_t addr);

	static __uint32_t getSecondIndex(__uint32_t addr);

	static __uint32_t getOffset(__uint32_t addr);

	void addPage(__uint32_t addr);

	bool isAddrExist(__uint32_t addr);

public:
	MemoryManager();

	~MemoryManager();

	void setByte(__uint32_t addr, __uint8_t val);

	__uint8_t getByte(__uint32_t addr);

	__uint32_t getInt(__uint32_t addr);
};


#endif //RISC_V_MEMORYMANAGER_H
