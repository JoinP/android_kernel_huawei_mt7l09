

#ifndef __MED_ANR_2MIC_COMM_H__
#define __MED_ANR_2MIC_COMM_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include"codec_com_codec.h"
#include"med_pp_comm.h"
#include "med_fft.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define MED_ANR_2MIC_FFT_LEN_NB             (320)                               /* ANR_2MIC中窄带FFT变换长度*/
#define MED_ANR_2MIC_FFT_LEN_WB             (640)                               /* ANR_2MIC中宽带FFT变换长度*/
#define MED_ANR_2MIC_PSD_BIN_LEN_MAX        (321)

/* 临时全局数组，供ANR_2MIC各模块使用 */
extern VOS_INT16                        g_ashwMedAnr2MicTmp1Len640[MED_ANR_2MIC_FFT_LEN_WB];

/* ANR_2MIC 公共宏定义*/
#define MED_ANR_2MIC_GetshwVecTmp640Ptr1()   (&g_ashwMedAnr2MicTmp1Len640[0])   /* 临时全局数组指针 长度640 INT16 */

#define CODEC_OpLSmooth(swA, shwAlpha, swB, shwSubAlpha)      \
    (CODEC_OpL_add(CODEC_OpL_mpy_32_16(swA, shwAlpha),  CODEC_OpL_mpy_32_16(swB, shwSubAlpha)))

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : MED_2MIC_NV_STRU
 功能描述  : 描述2MIC模块NV项的结构体
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enEnable;                               /* 2MIC使能开关 */
    CODEC_SWITCH_ENUM_UINT16            enCalibEnable;                          /* 2MIC校准模块使能开关 */
    VOS_INT16                           shwVadRatioThld;                        /* Vad检测判别阈值 */
    VOS_INT16                           shwRcDelay;                             /* 辅麦克延迟 */
    VOS_INT16                           shwTimeAlpha;                           /* 计算平滑功率谱时的平滑系数 */
    VOS_INT16                           shwSumProbThld;                         /* 全带语音存在概率阈值 */
    VOS_INT16                           shwGainMin;                             /* 增益下限 */
    VOS_INT16                           shwNsLowTh;                             /* 噪声判决低门限 */
    VOS_INT16                           shwNsHighTh;                            /* 噪声判决高门限 */
    VOS_INT16                           shwSpLowTh;                             /* 语音存在概率低门限 */
    VOS_INT16                           shwFarMuteCntThd;                       /* 无远端信号帧计数判决门限*/
    VOS_INT16                           shwFarMaxThd1;                          /* 远端信号门限1，低于此门限则进行LAF滤波 */
    VOS_INT16                           shwFarMaxThd2;                          /* 远端信号门限2，超过此门限则在FD将参考信号置零 */
} MED_2MIC_NV_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_COMM_STRU
 功能描述  : 描述2MIC公共参数的结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwFftLen;                              /* FFT变换长度*/
	MED_FFT_NUM_ENUM_INT16              enFftNumIndex;                          /* FFT长度索引枚举 */
	MED_FFT_NUM_ENUM_INT16              enIfftNumIndex;                         /* IFFT长度索引枚举 */
} MED_2MIC_COMM_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_FREQ
 功能描述  : 描述频谱相关数据的结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                  ashwFreq[MED_ANR_2MIC_FFT_LEN_WB];             /* 频域信号 */
    VOS_INT16                  ashwOverlapFft[CODEC_FRAME_LENGTH_WB];       /* FFT 重叠信号*/
    VOS_INT16                  shwNormShift;                                    /* 归一化系数*/
} MED_2MIC_FREQ_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_DATAFLOW_MC_STRU
 功能描述  : 描述主麦克数据相关的结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                  ashwOverlapIfft[CODEC_FRAME_LENGTH_WB];      /* IFFT 重叠信号*/
    MED_2MIC_FREQ_STRU         stMc;                                            /* 主麦克信号 */
    MED_2MIC_FREQ_STRU         stOriMc;
} MED_2MIC_DATAFLOW_MC_STRU;

/*****************************************************************************
 实体名称  : MED_2MIC_DATAFLOW_RC_STRU
 功能描述  : 描述辅麦克数据相关的结构体
*****************************************************************************/
typedef struct
{
    MED_2MIC_FREQ_STRU         stRc;
    MED_2MIC_FREQ_STRU         stOriRc;
} MED_2MIC_DATAFLOW_RC_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/












#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_anr_2mic_comm.h */
