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

/*
 *	Initial the global branch history register
 *	input	:
 *		history_width	:	the width of history pattern in global branch history register
 *							(gshare / hybrid) h
 */
void GHR_Initial(GHR *GlobalBranchHistoryRegister, uint32_t history_width);

/*
 *	Update the GlobalBranchHistoryRegister
 *	input	:
 *		result	:	struct "Result", the prediction and actual result
 */
void GHR_Update(GHR *GlobalBranchHistoryRegister, Result result);

/*
 * Print the content of GlobalBranchHistoryRegister to file *fp
 */
void GHR_fprintf(GHR *GlobalBranchHistoryRegister, FILE *fp);

#endif