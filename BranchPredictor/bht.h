/*
 *	branch history table
 */
#pragma once

typedef struct BHT_Attributes
{
	uint32_t history_num;
	uint32_t index_width;
	uint32_t history_width;
	uint64_t history_max;
}BHT_Attributes;

typedef struct BHT
{
	uint64_t* history;
	BHT_Attributes attributes;
}BHT;