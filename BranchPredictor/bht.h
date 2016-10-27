/*
 *	branch history table
 */
#pragma once

typedef struct BHT_Attributes
{
	uint32_t history_num;
	uint32_t index_width;
	uint32_t history_width;
	uint64_t history_one;
}BHT_Attributes;

typedef struct BHT
{
	uint64_t* history;
	BHT_Attributes attributes;
}BHT;

void BHT_Initial(BHT *BranchHistoryTable, uint32_t index_width, uint32_t history_width);

uint64_t BHT_Search(BHT *BranchHistoryTable, uint32_t addr);

void BHT_Update(BHT *BranchHistoryTable, uint32_t addr, Result result);

void BHT_fprintf(BHT *BranchHistoryTable, FILE *fp);