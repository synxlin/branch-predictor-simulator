/*
 *	branch target buffer
 */

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "btb.h"

 /*
  *	Inital the branch target buffer
  */
void BTB_Initial(BTB* BranchTargetBuffer, uint32_t assoc, uint32_t index_width)
{
	/* first, initial the attributes of BTB */
	BranchTargetBuffer->attributes.assoc = assoc;
	BranchTargetBuffer->attributes.set_num = (uint32_t)pow_2(index_width);
	
	BranchTargetBuffer->attributes.index_width = index_width;
	BranchTargetBuffer->attributes.tag_width = 30 - index_width;

	/* then, allocate space for sets (including blocks and tag array) */
	BranchTargetBuffer->set = (Set *)malloc(sizeof(Set) * BranchTargetBuffer->attributes.set_num);
	if (BranchTargetBuffer->set == NULL)
		_error_exit("malloc")
	uint32_t i;
	for (i = 0; i < BranchTargetBuffer->attributes.set_num; i++)
	{
		BranchTargetBuffer->set[i].block = (Block *)malloc(sizeof(Block) * BranchTargetBuffer->attributes.assoc);
		if (BranchTargetBuffer->set[i].block == NULL)
			_error_exit("malloc")
		memset(BranchTargetBuffer->set[i].block, 0, sizeof(Block) * BranchTargetBuffer->attributes.assoc);
		BranchTargetBuffer->set[i].rank = (uint64_t *)malloc(sizeof(uint64_t) * BranchTargetBuffer->attributes.assoc);
		if (BranchTargetBuffer->set[i].rank == NULL)
			_error_exit("malloc")
		memset(BranchTargetBuffer->set[i].rank, 0, sizeof(uint64_t) * BranchTargetBuffer->attributes.assoc);
	}
	memset(&(BranchTargetBuffer->stat), 0, sizeof(BTB_Stat));
}

/*
 *	Interpreter the address into tag, index
 */
void Interpret_Address(BTB* BranchTargetBuffer, uint32_t addr, uint32_t *tag, uint32_t *index)
{
	uint32_t tag_width = BranchTargetBuffer->attributes.tag_width;
	*tag = addr >> (32 - tag_width);
	*index = (addr << tag_width) >> (tag_width + 2);
}

/*
 *	Rebuild address from tag and index
 */
uint32_t Rebuild_Address(BTB* BranchTargetBuffer, uint32_t tag, uint32_t index)
{
	uint32_t ADDR = 0;
	ADDR |= (tag << (BranchTargetBuffer->attributes.index_width + 2));
	ADDR |= (index << 2);
	return ADDR;
}

/*
 *	Search the BranchTargetBuffer
 *	return the way_num if HIT, otherwise, return the assoc
 */
uint32_t BTB_Search(BTB* BranchTargetBuffer, uint64_t tag, uint32_t index)//, uint32_t *way_num)
{
	uint32_t i, k = BranchTargetBuffer->attributes.assoc;
	for (i = 0; i < BranchTargetBuffer->attributes.assoc; i++)
		if (BranchTargetBuffer->set[index].block[i].valid_bit == VALID && BranchTargetBuffer->set[index].block[i].tag == tag)
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
void Rank_Maintain(BTB* BranchTargetBuffer, uint32_t index, uint32_t way_num, uint64_t rank_value)
{
	BranchTargetBuffer->set[index].rank[way_num] = rank_value;
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
uint32_t Rank_Top(BTB* BranchTargetBuffer, uint32_t index)
{
	uint32_t i, assoc = BranchTargetBuffer->attributes.assoc;
	/* we first use invalid block location */
	for (i = 0; i < assoc; i++)
		if (BranchTargetBuffer->set[index].block[i].valid_bit == INVALID)
			return i;
	uint64_t *rank = BranchTargetBuffer->set[index].rank;
	uint32_t k = 0;
	for (i = 0; i < assoc; i++)
		if (rank[i] < rank[k])
			k = i;
	return k;
}

/*
 *	Allocate (Place or Replace) block "blk" on BranchTargetBuffer, set "index", way "way_num"
 */
void BTB_Replacement(BTB* BranchTargetBuffer, uint32_t index, uint32_t way_num, uint32_t tag)
{
	BranchTargetBuffer->set[index].block[way_num].valid_bit = VALID;
	BranchTargetBuffer->set[index].block[way_num].tag = tag;
#ifdef DBG
	fprintf(debug_fp, "Replacement %lx: BranchTargetBuffer Set %u, Way %u\n", Rebuild_Address(blk.TAG, index), index, way_num);
#endif
}

/*
 *	Search the BranchTargetBuffer for PC "addr" and make prediction
 *	input	:
 *		addr	:	PC
 *	return	:
 *		the prediction on whether inst is branch -- BRANCH or NOT_BRANCH
 */
Branch_Result BTB_Predict(BTB* BranchTargetBuffer, uint32_t addr)
{
	uint32_t tag, index;
	Interpret_Address(BranchTargetBuffer, addr, &tag, &index);
	uint32_t way_num = BTB_Search(BranchTargetBuffer, tag, index);
	BranchTargetBuffer->stat.num_predictions++;
	if (way_num == BranchTargetBuffer->attributes.assoc)
		return not_branch;
	BranchTargetBuffer->stat.num_predict_branch++;
	return branch;
}

/*
 *	Update the BranchTargetBuffer
 *	input	:
 *		addr		:	PC
 *		result		:	struct "Result", the prediction and actual result
 *		rank_value	:	the index of the instr
 */
void BTB_Update(BTB* BranchTargetBuffer, uint32_t addr, Result result, uint64_t rank_value)
{
	uint32_t tag, index, way_num;
	/* if predition is correct */
	if (result.actual_branch == result.predict_branch)
	{
		if (result.actual_branch == not_branch)
			/* if it is not a branch and predition is correct, we do nothing */
			return;
		/* if it is a branch and predition is correct, we update the LRU bit */
		Interpret_Address(BranchTargetBuffer, addr, &tag, &index);
		way_num = BTB_Search(BranchTargetBuffer, tag, index);
	}
	/* if predition is not correct */
	else
	{
		/*
		 * Branch Target Branch is empty at first and branch instr is allocated in BTB
		 * only when it is proved to be a branch, so the misprediction can only be the
		 * case where predition is not a branch but it actually is.
		 * Under this situation, we need to replace block and update LRU bit.
		 */
		BranchTargetBuffer->stat.num_mispredict_branch++;
		Interpret_Address(BranchTargetBuffer, addr, &tag, &index);
		way_num = Rank_Top(BranchTargetBuffer, index);
		BTB_Replacement(BranchTargetBuffer, index, way_num, tag);
	}
	Rank_Maintain(BranchTargetBuffer, index, way_num, rank_value);
	BranchTargetBuffer->stat.num_updates++;
}

void BTB_fprintf(BTB* BranchTargetBuffer, FILE *fp)
{
	uint32_t i;
	for (i = 0; i < BranchTargetBuffer->attributes.set_num; i++)
	{
		fprintf(fp, "Set\t\t%u: ", i);
		uint32_t j;
		for (j = 0; j < BranchTargetBuffer->attributes.assoc; j++)
		{
			fprintf(fp, "  {%u, 0x  %x}", BranchTargetBuffer->set[i].block[j].valid_bit, Rebuild_Address(BranchTargetBuffer, BranchTargetBuffer->set[i].block[j].tag, i));
		}
		fprintf(fp, "\n");
	}
}