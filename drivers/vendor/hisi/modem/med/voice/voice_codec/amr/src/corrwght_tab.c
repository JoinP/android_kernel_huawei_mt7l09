/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : corrwght.tab
*      Purpose          : Tables for correlation weights
*      $Id $
*
********************************************************************************
*/
#include "codec_op_etsi.h"
#include "cnst.h"

static const Word16 corrweight[251] = {
	20473,	20506,	20539,	20572,	20605,	20644,	20677,
	20716,	20749,	20788,	20821,	20860,	20893,	20932,
	20972,	21011,	21050,	21089,	21129,	21168,	21207,
	21247,	21286,	21332,	21371,	21417,	21456,	21502,
	21542,	21588,	21633,	21679,	21725,	21771,	21817,
	21863,	21909,	21961,	22007,	22059,	22105,	22158,
	22210,	22263,	22315,	22367,	22420,	22472,	22531,
	22584,	22643,	22702,	22761,	22820,	22879,	22938,
	23003,	23062,	23128,	23193,	23252,	23324,	23390,
	23455,	23527,	23600,	23665,	23744,	23816,	23888,
	23967,	24045,	24124,	24202,	24288,	24366,	24451,
	24537,	24628,	24714,	24805,	24904,	24995,	25094,
	25192,	25297,	25395,	25500,	25611,	25723,	25834,
	25952,	26070,	26188,	26313,	26444,	26575,	26706,
	26844,	26988,	27132,	27283,	27440,	27597,	27761,
	27931,	28108,	28285,	28475,	28665,	28869,	29078,
	29295,	29524,	29760,	30002,	30258,	30527,	30808,
	31457,	32767,	32767,	32767,	32767,	32767,
	32767,	32767,	31457,	30808,	30527,	30258,	30002,
	29760,	29524,	29295,	29078,	28869,	28665,	28475,
	28285,	28108,	27931,	27761,	27597,	27440,	27283,
	27132,	26988,	26844,	26706,	26575,	26444,	26313,
	26188,	26070,	25952,	25834,	25723,	25611,	25500,
	25395,	25297,	25192,	25094,	24995,	24904,	24805,
	24714,	24628,	24537,	24451,	24366,	24288,	24202,
	24124,	24045,	23967,	23888,	23816,	23744,	23665,
	23600,	23527,	23455,	23390,	23324,	23252,	23193,
	23128,	23062,	23003,	22938,	22879,	22820,	22761,
	22702,	22643,	22584,	22531,	22472,	22420,	22367,
	22315,	22263,	22210,	22158,	22105,	22059,	22007,
	21961,	21909,	21863,	21817,	21771,	21725,	21679,
	21633,	21588,	21542,	21502,	21456,	21417,	21371,
	21332,	21286,	21247,	21207,	21168,	21129,	21089,
	21050,	21011,	20972,	20932,	20893,	20860,	20821,
	20788,	20749,	20716,	20677,	20644,	20605,	20572,
	20539,	20506,	20473,	20434,	20401,	20369,	20336};
