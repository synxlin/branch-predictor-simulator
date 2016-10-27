/*
 *	globle branch history register
 */
#pragma once

typedef struct GHR_Attributes
{
	uint32_t history_width;
	uint64_t history_max;
}GHR_Attributes;

typedef struct GHR
{
	uint64_t history;
	GHR_Attributes attributes;
}GHR;

void GHR_Initial(GHR *GlobalBranchHistoryTable, uint32_t history_width);

void GHR_Update(GHR *GlobalBranchHistoryTable, Result result);

