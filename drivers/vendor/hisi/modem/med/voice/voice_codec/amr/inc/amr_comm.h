

#ifndef __AMR_COMM_H__
#define __AMR_COMM_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#include "codec_op_etsi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/



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
extern Word32 AMR_OpVcMac4(
                Word16                (*pshwX)[40],
                Word32                  swSum,
                Word32                  swCnst,
                Word32                  swCol,
                Word32                  swRow0,
                Word32                  swRow1,
                Word32                  swRow2,
                Word32                  swRow3 );

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of amr_comm.h */
