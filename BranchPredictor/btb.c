/*
 *	branch target buffer
 */

#include <stdio.h>
#include <stdlib.h>
#include "btb.h"
#include "utils.h"

 /*
  *	Inital the branch target buffer
  */
void BTB_Initial(btb* BranchTargetBuffer, uint32_t assoc, uint32_t index_width)
{
	/* first, initial the attributes of BTB */
	BranchTargetBuffer->BTB_ATTRIBUTES.ASSOC = assoc;
	BranchTargetBuffer->BTB_ATTRIBUTES.SET_NUM = pow_2(index_width);

	BranchTargetBuffer->BTB_ATTRIBUTES.INDEX_WIDTH = index_width;
	BranchTargetBuffer->BTB_ATTRIBUTES.TAG_WIDTH = 30 - index_width;

	/* then, allocate space for sets (including blocks and tag array) */
	BranchTargetBuffer->SET = (set *)malloc(sizeof(set) * BranchTargetBuffer->BTB_ATTRIBUTES.SET_NUM);
	if (BranchTargetBuffer->SET == NULL)
		_error_exit("malloc")
	uint32_t i;
	for (i = 0; i < BranchTargetBuffer->BTB_ATTRIBUTES.SET_NUM; i++)
	{
		BranchTargetBuffer->SET[i].BLOCK = (block *)malloc(sizeof(block) * BranchTargetBuffer->BTB_ATTRIBUTES.ASSOC);
		if (BranchTargetBuffer->SET[i].BLOCK == NULL)
			_error_exit("malloc")
			memset(BranchTargetBuffer->SET[i].BLOCK, 0, sizeof(block) * BranchTargetBuffer->BTB_ATTRIBUTES.ASSOC);
		BranchTargetBuffer->SET[i].RANK = (uint64_t *)malloc(sizeof(uint64_t) * BranchTargetBuffer->BTB_ATTRIBUTES.ASSOC);
		if (BranchTargetBuffer->SET[i].RANK == NULL)
			_error_exit("malloc")
			memset(BranchTargetBuffer->SET[i].RANK, 0, sizeof(uint64_t) * BranchTargetBuffer->BTB_ATTRIBUTES.ASSOC);
	}
}

/*
 *	Interpreter the address into tag, index
 */
void Interpret_Address(btb* BranchTargetBuffer, uint32_t ADDR, uint32_t *tag, uint32_t *index)
{
	uint32_t tag_width = BranchTargetBuffer->BTB_ATTRIBUTES.TAG_WIDTH;
	*tag = ADDR >> (32 - tag_width);
	*index = (ADDR << tag_width) >> (tag_width + 2);
}

/*
 *	Rebuild address from tag and index
 */
uint32_t Rebuild_Address(btb* BranchTargetBuffer, uint32_t tag, uint32_t index)
{
	uint32_t ADDR = 0;
	ADDR |= (tag << (BranchTargetBuffer->BTB_ATTRIBUTES.INDEX_WIDTH + 2));
	ADDR |= (index << 2);
	return ADDR;
}

/*
 *	Search the BranchTargetBuffer
 *	return the result: HIT or MISS
 *	if HIT, the hit way number will return by input parameter pointer "*way_num"
 */
uint32_t BTB_Search(btb* BranchTargetBuffer, uint64_t tag, uint32_t index)//, uint32_t *way_num)
{
	uint32_t i, k = BranchTargetBuffer->BTB_ATTRIBUTES.ASSOC;
	for (i = 0; i < BranchTargetBuffer->BTB_ATTRIBUTES.ASSOC; i++)
		if (BranchTargetBuffer->SET[index].BLOCK[i].VALID_BIT == VALID && BranchTargetBuffer->SET[index].BLOCK[i].TAG == tag)
		{
			k = i;
			break;
		}
	return k;
	/*
	if (k == BranchTargetBuffer->BTB_ATTRIBUTES.ASSOC)
		return MISS;
	else
	{
		*way_num = k;
		return HIT;
	}
	*/
}

/*
 *	Maintain the rank array
 */
void Rank_Maintain(btb* BranchTargetBuffer, uint32_t index, uint32_t way_num, uint64_t rank_value)
{
	uint64_t *rank = BranchTargetBuffer->SET[index].RANK;
	rank[way_num] = rank_value;
#ifdef DBG
	{
		fprintf(debug_fp, "Rank: BranchTargetBuffer Set %u -- ", index);
		uint32_t i;
		for (i = 0; i < BranchTargetBuffer->BTB_ATTRIBUTES.ASSOC; i++)
			fprintf(debug_fp, "%llu ", rank[i]);
		fprintf(debug_fp, "\n");
	}
#endif
}

/*
 *	Return the way number to be placed or replaced
 */
uint32_t Rank_Top(btb* BranchTargetBuffer, uint32_t index)
{
	uint32_t i, assoc = BranchTargetBuffer->BTB_ATTRIBUTES.ASSOC;
	/* we first use invalid block location */
	for (i = 0; i < assoc; i++)
		if (BranchTargetBuffer->SET[index].BLOCK[i].VALID_BIT == INVALID)
			return i;
	uint64_t *rank = BranchTargetBuffer->SET[index].RANK;
	uint32_t k = 0;
	for (i = 0; i < assoc; i++)
		if (rank[i] < rank[k])
			k = i;
	return k;
}

/*
 *	Allocate (Place or Replace) block "blk" on BranchTargetBuffer, set "index", way "way_num"
 */
void BTB_Replacement(btb* BranchTargetBuffer, uint32_t index, uint32_t way_num, uint32_t tag)
{
	BranchTargetBuffer->SET[index].BLOCK[way_num].VALID_BIT = VALID;
	BranchTargetBuffer->SET[index].BLOCK[way_num].TAG = tag;
#ifdef DBG
	fprintf(debug_fp, "Replacement %lx: BranchTargetBuffer Set %u, Way %u\n", Rebuild_Address(blk.TAG, index), index, way_num);
#endif
}

/*
 *	Lookup the BranchTargetBuffer for PC "ADDR"
 *	input	:
 *		ADDR	:	PC
 *	return	:
 *		the prediction on whether inst is branch -- BRANCH or NOT_BRANCH
 */
uint8_t BTB_Lookup(btb* BranchTargetBuffer, uint32_t ADDR)
{
	uint32_t tag, index;
	Interpret_Address(BranchTargetBuffer, ADDR, &tag, &index);
	uint32_t way_num = BTB_Search(BranchTargetBuffer, tag, index);
	if (way_num == BranchTargetBuffer->BTB_ATTRIBUTES.ASSOC)
		return NOT_BRANCH;
	return BRANCH;
}

/*
 *	Update the BranchTargetBuffer
 *	input	:
 *		ADDR		:	PC
 *		result		:	whether is right or not, the prediction on whether inst is branch
 *		rank_value	:	the index of the instr
 */
void BTB_Update(btb* BranchTargetBuffer, uint32_t ADDR, uint8_t result, uint64_t rank_value)
{
	uint32_t tag, index, way_num;
	Interpret_Address(BranchTargetBuffer, ADDR, &tag, &index);
	if (result == CORRECT)
		way_num = BTB_Search(BranchTargetBuffer, tag, index);
	else
	{
		way_num = Rank_Top(BranchTargetBuffer, index);
		BTB_Replacement(BranchTargetBuffer, index, way_num, tag);
	}
	Rank_Maintain(BranchTargetBuffer, index, way_num, rank_value);
}
