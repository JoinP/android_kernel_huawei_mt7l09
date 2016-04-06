
#ifndef __DRX_MSP_API_H__
#define __DRX_MSP_API_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include  "vos.h"
#pragma   pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/
#define  	DRX_MSP_VOTE_SLEEP_OK	(0)

#define  	DRX_MSP_VOTE_LOCK		(1)
#define  	DRX_MSP_VOTE_UNLOCK		(0)

/* unvote to sleep */
#define set_drx_status_bit(bit)\
	do{\
		 (gbl_drx_msp_vote_status = gbl_drx_msp_vote_status | bit );\
	}while(0)

/* vote to sleep */
#define clear_drx_status_bit(bit)\
	do{\
		 (gbl_drx_msp_vote_status = gbl_drx_msp_vote_status & (~bit) );\
	}while(0)

/* read status */
#define  read_drx_status_bit( bit)    (gbl_drx_msp_vote_status & (bit))

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/

/*****************************************************************************
   5 STRUCT
*****************************************************************************/
typedef struct
{
	VOS_UINT32 	drx_msp_cur_vote_status;
}drx_msp_debug_vote_s;


/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
extern void drx_msp_global_debug(void);

/*****************************************************************************
  9 OTHERS
*****************************************************************************/



#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of diag_api.h */
