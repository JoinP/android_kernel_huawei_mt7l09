/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : reorder.h
*      Purpose          : To make sure that the LSFs are properly ordered
*                       : and to keep a certain minimum distance between
*                       : adjacent LSFs.
*
********************************************************************************
*/
#ifndef reorder_h
#define reorder_h "$Id $"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "codec_op_etsi.h"

/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
void Reorder_lsf (
    Word16 *lsf,       /* (i/o)  : vector of LSFs   (range: 0<=val<=0.5)    */
    Word16 min_dist,   /* (i)    : minimum required distance                */
    Word16 n           /* (i)    : LPC order                                */
);

#endif
