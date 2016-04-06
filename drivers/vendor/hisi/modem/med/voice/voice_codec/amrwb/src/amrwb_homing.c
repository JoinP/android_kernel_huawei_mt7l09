/*------------------------------------------------------------------------*
 *                         HOMING.C                                       *
 *------------------------------------------------------------------------*
 * Performs the homing routines                                           *
 *------------------------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "amrwb_cnst.h"
#include "amrwb_bits.h"

#define DHF_PARMS_MAX 32 /* homing frame pattern             */
#define NUM_OF_SPMODES 9

#define PRML 15
#define PRMN_7k NBBITS_7k/PRML + 1
#define PRMN_9k NBBITS_9k/PRML + 1
#define PRMN_12k NBBITS_12k/PRML + 1
#define PRMN_14k NBBITS_14k/PRML + 1
#define PRMN_16k NBBITS_16k/PRML + 1
#define PRMN_18k NBBITS_18k/PRML + 1
#define PRMN_20k NBBITS_20k/PRML + 1
#define PRMN_23k NBBITS_23k/PRML + 1
#define PRMN_24k NBBITS_24k/PRML + 1

Word16 prmnofsf[NUM_OF_SPMODES]=
{
    63,  81, 100,
   108, 116, 128,
   136, 152, 156
};


static const Word16 dfh_M7k[PRMN_7k] =
{
  3168, 29954, 29213, 16121,
    64, 13440, 30624, 16430,
 19008
};

static const Word16 dfh_M9k[PRMN_9k] =
{
   3168, 31665,  9943, 9123,
  15599,  4358, 20248, 2048,
  17040, 27787, 16816, 13888
};

static const Word16 dfh_M12k[PRMN_12k] =
{
  3168, 31665,  9943,  9128,
  3647,  8129, 30930, 27926,
 18880, 12319,   496,  1042,
  4061, 20446, 25629, 28069,
 13948
};

static const Word16 dfh_M14k[PRMN_14k] =
{
    3168, 31665,  9943,  9131,
   24815,   655, 26616, 26764,
    7238, 19136,  6144,    88,
    4158, 25733, 30567, 30494,
    221, 20321, 17823
};

static const Word16 dfh_M16k[PRMN_16k] =
{
    3168, 31665,  9943,  9131,
   24815,   700,  3824,  7271,
   26400,  9528,  6594, 26112,
     108,  2068, 12867, 16317,
   23035, 24632,  7528,  1752,
    6759, 24576
};

static const Word16 dfh_M18k[PRMN_18k] =
{
     3168, 31665,  9943,  9135,
    14787, 14423, 30477, 24927,
    25345, 30154,   916,  5728,
    18978,  2048,   528, 16449,
     2436,  3581, 23527, 29479,
     8237, 16810, 27091, 19052,
        0
};

static const Word16 dfh_M20k[PRMN_20k] =
{
     3168, 31665,  9943,  9129,
     8637, 31807, 24646,   736,
    28643,  2977,  2566, 25564,
    12930, 13960,  2048,   834,
     3270,  4100, 26920, 16237,
    31227, 17667, 15059, 20589,
    30249, 29123, 0
};

static const Word16 dfh_M23k[PRMN_23k] =
{
     3168, 31665,  9943,  9132,
    16748,  3202, 28179, 16317,
    30590, 15857, 19960,  8818,
    21711, 21538,  4260, 16690,
    20224,  3666,  4194,  9497,
    16320, 15388,  5755, 31551,
    14080,  3574, 15932,    50,
    23392, 26053, 31216
};

static const Word16 dfh_M24k[PRMN_24k] =
{
     3168, 31665,  9943,  9134,
    24776,  5857, 18475, 28535,
    29662, 14321, 16725,  4396,
    29353, 10003, 17068, 20504,
      720,     0,  8465, 12581,
    28863, 24774,  9709, 26043,
     7941, 27649, 13965, 15236,
    18026, 22047, 16681,  3968
};


/* overall table with the parameters of the
   decoder homing frames for all modes */

static const Word16 *dhf[] =
{
  dfh_M7k,
  dfh_M9k,
  dfh_M12k,
  dfh_M14k,
  dfh_M16k,
  dfh_M18k,
  dfh_M20k,
  dfh_M23k,
  dfh_M24k,
  dfh_M24k
};

Word16 AMRWB_encoder_homing_frame_test(Word16 input_frame[])
{
    Word16 j = 0;
    Word32 i;

    /* check 320 input samples for matching EHF_MASK: defined in e_homing.h */
    for (i = 0; i < L_FRAME16k; i++)
    {
        j = (Word16) (input_frame[i] ^ AMRWB_EHF_MASK);

        if (j)
            break;
    }

    return (Word16) (!j);
}

static Word16 dhf_test(Word16 input_frame[], Word16 mode, Word16 nparms)
{
    Word16 j, tmp, shift;
    Word32 i;
    Word16 param[DHF_PARMS_MAX];
    Word16 *prms;

    prms = input_frame;
    j = 0;
    i = 0;

    if (sub(mode, MRDTX) != 0)
    {
        if (sub(mode, MODE_24k) != 0)
        {
            /* convert the received serial bits */
            tmp = sub(nparms, 15);
            while (sub(tmp, j) > 0)
            {
                param[i] = Serial_parm(15, &prms);
                j = add(j, 15);
                i = add(i, 1);
            }
            tmp = sub(nparms, j);
            param[i] = Serial_parm(tmp, &prms);
            shift = sub(15, tmp);
            param[i] = shl(param[i], shift);
        }
        else
        {
            /*If mode is 23.85Kbit/s, remove high band energy bits */
            for (i = 0; i < 10; i++)
            {
                param[i] = Serial_parm(15, &prms);
            }
            param[10] = Serial_parm(15, &prms) & 0x61FF;
            for (i = 11; i < 17; i++)
            {
                param[i] = Serial_parm(15, &prms);
            }
            param[17] = Serial_parm(15, &prms) & 0xE0FF;
            for (i = 18; i < 24; i++)
            {
                param[i] = Serial_parm(15, &prms);
            }
            param[24] = Serial_parm(15, &prms) & 0x7F0F;
            for (i = 25; i < 31; i++)
            {
                param[i] = Serial_parm(15, &prms);
            }
            tmp = Serial_parm(8, &prms);
            param[31] = shl(tmp,7);
            shift=0;
        }

        /* check if the parameters matches the parameters of the corresponding decoder homing frame */
        tmp = i;
        j = 0;
        for (i = 0; i < tmp; i++)
        {
            j = (Word16) (param[i] ^ dhf[mode][i]);
            if (j)
                break;
        }
        tmp = 0x7fff;
        tmp = shr(tmp, shift);
        tmp = shl(tmp, shift);
        tmp = (Word16) (dhf[mode][i] & tmp);
        tmp = (Word16) (param[i] ^ tmp);
        j = (Word16) (j | tmp);

    }
    else
    {
        j = 1;
    }

    return (Word16) (!j);
}


Word16 AMRWB_decoder_homing_frame_test(Word16 input_frame[], Word16 mode)
{
    /* perform test for COMPLETE parameter frame */
    return dhf_test(input_frame, mode, nb_of_bits[mode]);
}


Word16 AMRWB_decoder_homing_frame_test_first(Word16 input_frame[], Word16 mode)
{
    /* perform test for FIRST SUBFRAME of parameter frame ONLY */
    return dhf_test(input_frame, mode, prmnofsf[mode]);
}
