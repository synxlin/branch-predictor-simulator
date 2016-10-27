/*
 *	branch chooser table
 */
#pragma once

#ifndef BCT_H_
#define BCT_H_

#define STRONGLY_GSHARE 3
#define WEAKLY_GSHARE 2
#define WEAKLY_BIMODAL 1
#define STRONGLY_BIMODAL 0

typedef enum Two_Bit_Chooser
{
	strongly_bimodal = STRONGLY_BIMODAL,
	weakly_bimodal = WEAKLY_BIMODAL,
	weakly_gshare = WEAKLY_GSHARE,
	strongly_gshare = STRONGLY_GSHARE
} Two_Bit_Chooser;

typedef struct BCT_Attributes
{
	uint32_t chooser_num;
	uint32_t index_width;
}BCT_Attributes;

typedef struct BCT
{
	Two_Bit_Chooser* chooser;
	BCT_Attributes attributes;
}BCT;

void BCT_Initial(BCT* BranchChooserTable, uint32_t index_width);

Predictor BCT_Predict(BCT* BranchChooserTable, uint32_t addr);

void BCT_Update(BCT* BranchChooserTable, uint32_t addr, Result result);

void BCT_fprintf(BCT* BranchChooserTable, FILE *fp);

#endif