/*-------------------------------------------------------------------*
 *                         ISP_ISF.C                                 *
 *-------------------------------------------------------------------*
 *   Isp_isf   Transformation isp to isf                             *
 *   Isf_isp   Transformation isf to isp                             *
 *                                                                   *
 * The transformation from isp[i] to isf[i] and isf[i] to isp[i] are *
 * approximated by a look-up table and interpolation.                *
 *-------------------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"

/* table of cos(x) in Q15 */

static Word16 table[129] = {
  32767,
  32758,  32729,  32679,  32610,  32522,  32413,  32286,  32138,
  31972,  31786,  31581,  31357,  31114,  30853,  30572,  30274,
  29957,  29622,  29269,  28899,  28511,  28106,  27684,  27246,
  26791,  26320,  25833,  25330,  24812,  24279,  23732,  23170,
  22595,  22006,  21403,  20788,  20160,  19520,  18868,  18205,
  17531,  16846,  16151,  15447,  14733,  14010,  13279,  12540,
  11793,  11039,  10279,   9512,   8740,   7962,   7180,   6393,
   5602,   4808,   4011,   3212,   2411,   1608,    804,      0,
   -804,  -1608,  -2411,  -3212,  -4011,  -4808,  -5602,  -6393,
  -7180,  -7962,  -8740,  -9512, -10279, -11039, -11793, -12540,
 -13279, -14010, -14733, -15447, -16151, -16846, -17531, -18205,
 -18868, -19520, -20160, -20788, -21403, -22006, -22595, -23170,
 -23732, -24279, -24812, -25330, -25833, -26320, -26791, -27246,
 -27684, -28106, -28511, -28899, -29269, -29622, -29957, -30274,
 -30572, -30853, -31114, -31357, -31581, -31786, -31972, -32138,
 -32286, -32413, -32522, -32610, -32679, -32729, -32758, -32768};

/* slope in Q11 used to compute y = acos(x) */

static Word16 slope[128] = {
 -26214, -9039, -5243, -3799, -2979, -2405, -2064, -1771,
 -1579, -1409, -1279, -1170, -1079, -1004, -933, -880,
 -827, -783, -743, -708, -676, -647, -621, -599,
 -576, -557, -538, -521, -506, -492, -479, -466,
 -456, -445, -435, -426, -417, -410, -402, -395,
 -389, -383, -377, -372, -367, -363, -359, -355,
 -351, -348, -345, -342, -340, -337, -335, -333,
 -331, -330, -329, -328, -327, -326, -326, -326,
 -326, -326, -326, -327, -328, -329, -330, -331,
 -333, -335, -337, -340, -342, -345, -348, -351,
 -355, -359, -363, -367, -372, -377, -383, -389,
 -395, -402, -410, -417, -426, -435, -445, -456,
 -466, -479, -492, -506, -521, -538, -557, -576,
 -599, -621, -647, -676, -708, -743, -783, -827,
 -880, -933, -1004, -1079, -1170, -1279, -1409, -1579,
 -1771, -2064, -2405, -2979, -3799, -5243, -9039, -26214};


void Isp_isf(
     Word16 isp[],                         /* (i) Q15 : isp[m] (range: -1<=val<1)                */
     Word16 isf[],                         /* (o) Q15 : isf[m] normalized (range: 0.0<=val<=0.5) */
     Word16 m                              /* (i)     : LPC order                                */
)
{
    Word16 i, ind;
    Word32 L_tmp;

    ind = 127;                               /* beging at end of table -1 */

    for (i = (Word16) (m - 1); i >= 0; i--)
    {
        if (sub(i, sub(m, 2)) >= 0)
        {                                  /* m-2 is a constant */
            ind = 127;                       /* beging at end of table -1 */
        }
        /* find value in table that is just greater than isp[i] */
        while (sub(table[ind], isp[i]) < 0)
            ind--;

        /* acos(isp[i])= ind*128 + ( ( isp[i]-table[ind] ) * slope[ind] )/2048 */

        L_tmp = L_mult(sub(isp[i], table[ind]), slope[ind]);
        isf[i] = round(L_shl(L_tmp, 4));   /* (isp[i]-table[ind])*slope[ind])>>11 */

        isf[i] = add(isf[i], shl(ind, 7));
    }

    isf[m - 1] = shr(isf[m - 1], 1);

    return;
}


void Isf_isp(
     Word16 isf[],                         /* (i) Q15 : isf[m] normalized (range: 0.0<=val<=0.5) */
     Word16 isp[],                         /* (o) Q15 : isp[m] (range: -1<=val<1)                */
     Word16 m                              /* (i)     : LPC order                                */
)
{
    Word16 i, ind, offset;
    Word32 L_tmp;

    /*for (i = 0; i < m - 1; i++)
    {
        isp[i] = isf[i];
    }*/
    CODEC_OpVecCpy(isp, isf, m-1);

    isp[m - 1] = shl(isf[m - 1], 1);

    for (i = 0; i < m; i++)
    {
        ind = shr(isp[i], 7);              /* ind    = b7-b15 of isf[i] */
        offset = (Word16) (isp[i] & 0x007f);      /* offset = b0-b6  of isf[i] */

        /* isp[i] = table[ind]+ ((table[ind+1]-table[ind])*offset) / 128 */

        L_tmp = L_mult(sub(table[ind + 1], table[ind]), offset);
        isp[i] = add(table[ind], extract_l(L_shr(L_tmp, 8)));
    }

    return;
}
