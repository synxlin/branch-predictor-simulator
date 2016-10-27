/*
 *	branch target buffer
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

void BTB_Init(uint32_t assoc, uint32_t index_width);

void Interpret_Address(uint32_t addr, uint32_t *tag, uint32_t *index);

uint32_t Rebuild_Address(uint32_t tag, uint32_t index);

uint32_t BTB_Search(uint64_t tag, uint32_t index);

void Rank_Maintain(uint32_t index, uint32_t way_num, uint64_t rank_value);

uint32_t Rank_Top(uint32_t index);

void BTB_Replacement(uint32_t index, uint32_t way_num, uint32_t tag);

Branch_Result BTB_Predict(uint32_t addr);

void BTB_Update(uint32_t addr, Result result, uint64_t rank_value);

void BTB_fprintf(FILE *fp);

#endif