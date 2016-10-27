/*
 *	globle branch history register
 */
#pragma once

#ifndef GHR_H_
#define GHR_H_

typedef struct GHR_Attributes
{
	uint32_t history_width;
	uint32_t history_one;
}GHR_Attributes;

typedef struct GHR
{
	uint32_t history;
	GHR_Attributes attributes;
}GHR;

void GHR_Initial(GHR *GlobalBranchHistoryRegister, uint32_t history_width);

void GHR_Update(GHR *GlobalBranchHistoryRegister, Result result);

void GHR_fprintf(GHR *GlobalBranchHistoryRegister, FILE *fp);

#endif