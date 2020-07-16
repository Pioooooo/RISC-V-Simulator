//
// Created by Pioooooo on 2020/7/11.
//

#ifndef CODE_MEMORYMANAGERLARGE_H
#define CODE_MEMORYMANAGERLARGE_H

#include <cstring>
#include "MemoryManager.h"

#define __MAX_FIRST_INDEX 1024
#define __MAX_SECOND_INDEX 1024
#define __PAGE_SIZE 4096

class MemoryManagerLarge: public MemoryManager
{
	__uint8_t ***memory;
	
	static __uint32_t getFirstIndex(__uint32_t addr);
	
	static __uint32_t getSecondIndex(__uint32_t addr);
	
	static __uint32_t getOffset(__uint32_t addr);
	
	void addPage(__uint32_t addr);
	
	bool isAddrExist(__uint32_t addr);

public:
	MemoryManagerLarge();
	
	~MemoryManagerLarge() override;
	
	void setByte(__uint32_t addr, __uint8_t val) override;
	
	void setShort(__uint32_t addr, __uint16_t val) override;
	
	void setInt(__uint32_t addr, __uint32_t val) override;
	
	__uint8_t getByte(__uint32_t addr) override;
	
	__uint16_t getShort(__uint32_t addr) override;
	
	__uint32_t getInt(__uint32_t addr) override;
	
};


#endif //CODE_MEMORYMANAGERLARGE_H
