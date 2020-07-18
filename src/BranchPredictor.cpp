//
// Created by Pioooooo on 2020/7/14.
//

#include "BranchPredictor.h"

BranchPredictor::BranchPredictor(): total(0), success(0)
{
	memset(predBuf, 0xAA, sizeof(__uint8_t) * PRED_BUF_SIZE);
}

__uint8_t BranchPredictor::hash(__uint32_t x)
{
	return x & 0xffu;
}

bool BranchPredictor::predict(__uint32_t pc)
{
	__uint8_t x = hash(pc);
	total++;
	return (predBuf[x >> 2u] >> ((x & 3u) << 1u)) & 2u;
}

void BranchPredictor::update(__uint32_t pc, bool branch)
{
	__uint8_t x = hash(pc), cur = (predBuf[x >> 2u] >> ((x & 3u) << 1u)) & 3u;
	if(!(cur & 2u) == !branch)
		success++;
	switch(cur)
	{
	case STRONG_NOT_TAKEN:
		if(branch)
			cur = WEAK_NOT_TAKEN;
		break;
	case WEAK_NOT_TAKEN:
		if(branch)
			cur = WEAK_TAKEN;
		else
			cur = STRONG_NOT_TAKEN;
		break;
	case WEAK_TAKEN:
		if(branch)
			cur = STRONG_TAKEN;
		else
			cur = WEAK_NOT_TAKEN;
		break;
	case STRONG_TAKEN:
		if(!branch)
			cur = WEAK_TAKEN;
		break;
	default:
		break;
	}
	predBuf[x >> 2u] &= ~(3u << ((x & 3u) << 1u));
	predBuf[x >> 2u] |= cur << ((x & 3u) << 1u);
}

void BranchPredictor::print(FILE *file)
{
	fprintf(file, "total:%8d,success:%8d,success rate:%8f\n", total, success, (double)success / total);
}

