/*
 *	branch target buffer
 */
#pragma once

#define NOT_BRANCH 0
#define BRANCH 1

#define VALID 1
#define INVALID 0

typedef struct block
{
	/* uint32_t* Target_Addr; */
	uint32_t TAG;
	uint8_t VALID_BIT;
}block;

typedef struct set
{
	block *BLOCK;
	uint64_t *RANK;
}set;

typedef struct btb_stat
{
	uint64_t num_access;
	uint64_t num_predict_branch;
	uint64_t num_mispredict_branch;
}btb_stat;

typedef struct btb_attributes
{
	uint32_t ASSOC;
	uint32_t SET_NUM;
	uint32_t TAG_WIDTH;
	uint32_t INDEX_WIDTH;
}btb_attributes;

typedef struct btb
{
	set *SET;
	btb_attributes BTB_ATTRIBUTES;
}btb;

void BTB_Initial(btb* BranchTargetBuffer, uint32_t assoc, uint32_t index_width);

void Interpret_Address(btb* BranchTargetBuffer, uint32_t ADDR, uint32_t *tag, uint32_t *index);

uint32_t Rebuild_Address(btb* BranchTargetBuffer, uint32_t tag, uint32_t index);

uint32_t BTB_Search(btb* BranchTargetBuffer, uint64_t tag, uint32_t index);

void Rank_Maintain(btb* BranchTargetBuffer, uint32_t index, uint32_t way_num, uint64_t rank_value);

uint32_t Rank_Top(btb* BranchTargetBuffer, uint32_t index);

void BTB_Replacement(btb* BranchTargetBuffer, uint32_t index, uint32_t way_num, uint32_t tag);

uint8_t BTB_Lookup(btb* BranchTargetBuffer, uint32_t ADDR);

void BTB_Update(btb* BranchTargetBuffer, uint32_t ADDR, uint8_t result, uint64_t rank_value);