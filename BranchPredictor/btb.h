/*
 *	Branch Target Buffer
 */
#pragma once

#ifndef BTB_H_
#define BTB_H_

typedef struct Block
{
	/* uint32_t* Target_Addr; */
	uint32_t tag;
	uint8_t valid_bit;
}Block;

typedef struct Set
{
	Block *block;
	uint64_t *rank;
}Set;

typedef struct BTB_Attributes
{
	uint32_t assoc;
	uint32_t set_num;
	uint32_t tag_width;
	uint32_t index_width;
}BTB_Attributes;

typedef struct BTB
{
	Set *set;
	BTB_Attributes attributes;
}BTB;

extern BTB* branch_target_buffer;

/*
 *	Initial the branch target buffer
 * input	:
 *		assoc		:	the associativity of Branch Target Buffer
 *		index_width	:	the width of index in PC, "i_BTB"
 */
void BTB_Init(uint32_t assoc, uint32_t index_width);

/*
 *	Interpreter the address into tag, index
 */
void Interpret_Address(uint32_t addr, uint32_t *tag, uint32_t *index);

/*
 *	Rebuild address from tag and index
 *	return	:
 *		addr	:	PC address
 */
uint32_t Rebuild_Address(uint32_t tag, uint32_t index);

/*
 *	Search the branch_target_buffer
 *	return	:
 *		way_num	:	if HIT, return correspounding way number; if MISS, return "assoc"
 */
uint32_t BTB_Search(uint64_t tag, uint32_t index);

/*
 *	Maintain the rank array
 */
void Rank_Maintain(uint32_t index, uint32_t way_num, uint64_t rank_value);

/*
 *	Return the way number to be placed or replaced
 */
uint32_t Rank_Top(uint32_t index);

/*
 *	Allocate (Place or Replace) block "blk" on branch_target_buffer, set "index", way "way_num"
 */
void BTB_Replacement(uint32_t index, uint32_t way_num, uint32_t tag);

/*
 *	Search the branch_target_buffer for PC "addr" and make prediction
 *	input	:
 *		addr	:	PC
 *	return	:
 *		the prediction on whether inst is branch -- BRANCH or NOT_BRANCH
 */
Branch_Result BTB_Predict(uint32_t addr);

/*
 *	Update the branch_target_buffer
 *	input	:
 *		addr		:	PC
 *		result		:	struct "Result", the prediction and actual result
 *		rank_value	:	the index of the instr
 */
void BTB_Update(uint32_t addr, Result result, uint64_t rank_value);

/*
 * Print the content of branch_target_buffer to file *fp
 */
void BTB_fprintf(FILE *fp);

#endif