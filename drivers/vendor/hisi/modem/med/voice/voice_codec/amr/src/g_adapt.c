/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : g_adapt.c
*      Purpose          : gain adaptation for MR795 gain quantization
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "g_adapt.h"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "cnst.h"
#include "gmed_n.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#define LTP_GAIN_THR1 2721 /* 2721 Q13 = 0.3322 ~= 1.0 / (10*log10(2)) */
#define LTP_GAIN_THR2 5443 /* 5443 Q13 = 0.6644 ~= 2.0 / (10*log10(2)) */

/* GainAdaptState 状态全局变量,在本模块的init函数中作为预先分配的内存替换malloc */
GainAdaptState g_stAmrGainAdaptState;

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  Function:   gain_adapt_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/
int gain_adapt_init (GainAdaptState **st)
{
    GainAdaptState* s;

    if (st == (GainAdaptState **) AMR_NULL){
        return -1;
    }
    *st = AMR_NULL;

    /* allocate memory */
    s = &g_stAmrGainAdaptState;

    gain_adapt_reset(s);
    *st = s;

    return 0;
}

/*************************************************************************
*
*  Function:   gain_adapt_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int gain_adapt_reset (GainAdaptState *st)
{
    if (st == (GainAdaptState *) AMR_NULL){
        return -1;
    }

    st->onset = 0;
    st->prev_alpha = 0;
    st->prev_gc = 0;

    CODEC_OpVecSet(&st->ltpg_mem[0],
                 LTPG_MEM_SIZE,
                 0);

    return 0;
}

/*************************************************************************
*
*  Function:   gain_adapt_exit
*  Purpose:    The memory used for state memory is freed
*
**************************************************************************
*/
void gain_adapt_exit (GainAdaptState **st)
{
    if (st == AMR_NULL || *st == AMR_NULL)
        return;

    *st = AMR_NULL;

    return;
}

/*************************************************************************
 *
 *  Function:   gain_adapt()
 *  Purpose:    calculate pitch/codebook gain adaptation factor alpha
 *              (and update the adaptor state)
 *
 **************************************************************************
 */
void gain_adapt(
    GainAdaptState *st,  /* i  : state struct                  */
    Word16 ltpg,         /* i  : ltp coding gain (log2()), Q13 */
    Word16 gain_cod,     /* i  : code gain,                Q1  */
    Word16 *alpha        /* o  : gain adaptation factor,   Q15 */
)
{
    Word16 adapt;      /* adaptdation status; 0, 1, or 2       */
    Word16 result;     /* alpha factor, Q13                    */
    Word16 filt;       /* median-filtered LTP coding gain, Q13 */
    Word16 tmp, i;

    /* basic adaptation */

    if ( ltpg <= (Word16)LTP_GAIN_THR1 )
    {
        adapt = 0;
    }
    else
    {

        if ( ltpg <= (Word16)LTP_GAIN_THR2 )
        {
            adapt = 1;
        }
        else
        {
            adapt = 2;
        }
    }

    /*
     * // onset indicator
     * if ((cbGain > onFact * cbGainMem[0]) && (cbGain > 100.0))
     *     onset = 8;
     * else
     *     if (onset)
     *         onset--;
     */
    /* tmp = cbGain / onFact; onFact = 2.0; 200 Q1 = 100.0 */
    tmp = shr_r (gain_cod, 1);

    if ((tmp > st->prev_gc) && (gain_cod > (Word16)200))
    {
        st->onset = 8;
    }
    else
    {

        if (st->onset != 0)
        {
            st->onset = sub (st->onset, 1);
        }
    }

    /*
     *  // if onset, increase adaptor state
     *  if (onset && (gainAdapt < 2)) gainAdapt++;
     */

    if ((st->onset != 0) && ( adapt < (Word16)2 ))
    {
        adapt = add (adapt, 1);
    }

    st->ltpg_mem[0] = ltpg;
    filt = gmed_n (st->ltpg_mem, 5);    /* function result */

    if (adapt == 0)
    {

        if ( filt > (Word16)5443 ) /* 5443 Q13 = 0.66443... */
        {
            result = 0;
        }
        else
        {

            if (filt < 0)
            {
                result = 16384;   /* 16384 Q15 = 0.5 */
            }
            else
            {   /* result       =   0.5 - 0.75257499*filt     */
                /* result (Q15) = 16384 - 24660 * (filt << 2) */
                filt = shl (filt, 2); /* Q15 */
                result = sub (16384, mult (24660, filt));
            }
        }
    }
    else
    {
        result = 0;
    }
    /*
     *  if (prevAlpha == 0.0) result = 0.5 * (result + prevAlpha);
     */

    if (st->prev_alpha == 0)
    {
        result = shr (result, 1);
    }

    /* store the result */
    *alpha = result;

    /* update adapter state memory */
    st->prev_alpha = result;
    st->prev_gc = gain_cod;

    for (i = LTPG_MEM_SIZE-1; i > 0; i--)
    {
        st->ltpg_mem[i] = st->ltpg_mem[i-1];
    }
    /* mem[0] is just present for convenience in calling the gmed_n[5]
     * function above. The memory depth is really LTPG_MEM_SIZE-1.
     */
}
