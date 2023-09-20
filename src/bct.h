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

/*
 *	Inital the branch chooser table
 *	input	:
 *		index_width	:	the width of index in PC addr to index the branch choose table
 *						(hybrid) i_C
 */
void BCT_Initial(BCT* BranchChooserTable, uint32_t index_width);

/*
 *	Search the BranchChooserTable for PC "addr" and make prediction
 *	input	:
 *		addr	:	PC
 *		two_byte_instr: Are instructions a mix of 4 and 2 byte length. Modifies index function.
 *	return	:
 *		the prediction on which predictor is chosen
 */
Predictor BCT_Predict(BCT* BranchChooserTable, uint32_t addr, uint8_t two_byte_inst);

/*
 *	Update the BranchChooserTable
 *	input	:
 *		addr	:	PC
 *		two_byte_instr: Are instructions a mix of 4 and 2 byte length. Modifies index function.
 *		result	:	struct "Result", the prediction and actual result
 */
void BCT_Update(BCT* BranchChooserTable, uint32_t addr, uint8_t two_byte_inst, Result result);

/*
 * Print the content of BranchChooserTable to file *fp
 */
void BCT_fprintf(BCT* BranchChooserTable, FILE *fp);

#endif
