
#include "rabminclude.h"
#include "AtRabmInterface.h"
#include "NasRabmMain.h"

#include "CdsRabmInterface.h"
#include "NasUtranCtrlInterface.h"
#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_RABM_SNDMSG_C

VOS_UINT32                              g_ulNasRabmStubRlcSduSize = 144;

extern VOS_UINT32 RLC_GetRbMaxUpPduSize(VOS_UINT8 ucRbId, VOS_UINT32 ulPduSize, VOS_UINT8 ucCrcSize);




VOS_VOID RABM_SndRabReestReq()
{
    GMMRABM_REESTABLISH_REQ_STRU *pSndMsg;
    VOS_UINT8  ucRrcCause;
    VOS_UINT8  ucTraClass;
    VOS_UINT8  i;
    VOS_UINT16                          usPdpContextStatus = 0;
    #if(PS_UE_REL_VER >= PS_PTL_VER_R7)
    VOS_UINT16  usUplinkDataStatus = 0;
    #endif

    pSndMsg = (GMMRABM_REESTABLISH_REQ_STRU *)
                PS_ALLOC_MSG(WUEPS_PID_RABM, sizeof(GMMRABM_REESTABLISH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pSndMsg)
    {
        PS_LOG(WUEPS_PID_RABM, VOS_NULL, PS_PRINT_ERROR, "RABM_SndRabReestReq:ERROR:Alloc msg fail!");
        return;
    }

    ucRrcCause = GMM_RABM_RRC_EST_CAUSE_ORG_BG_CALL;                            /* 初始化RRC建立原因                        */
    for(i = 0; i < RABM_PS_MAX_ENT_NUM; i++)
    {
        if(RABM_NULL != g_aRabmPsEnt[i].ucState)
        {                                                                       /* 实体状态为激活                           */
            ucTraClass = (g_aRabmPsEnt[i].QoS.aucQosValue[3] & NAS_RABM_QOS_TRAFFIC_CLASS_MASK)
                            >> NAS_RABM_QOS_TRAFFIC_CLASS_BIT_OFFSET;           /* 获得Traffic Class                        */
            if(( ucTraClass < ucRrcCause ) && ( 0 != ucTraClass ))
            {                                                                   /* RRC连接建立的优先级高于初始的优先级      */
                ucRrcCause = ucTraClass;                                        /* 更改RRC连接建立的优先级                  */
            }

            usPdpContextStatus |= (VOS_UINT16)(0x1 << (i + RABM_NSAPI_OFFSET));

            #if(PS_UE_REL_VER >= PS_PTL_VER_R7)
            if (RABM_NSAPI_ACTIVE_NO_RAB == g_aRabmPsEnt[i].ucState)            /*实体状态为激活未建RAB                     */
            {
                usUplinkDataStatus |= (VOS_UINT16)(0x1 << (i + RABM_NSAPI_OFFSET));         /*实体每一位置为1为每个PDP重建RAB           */
            }
            #endif
        }
    }
    pSndMsg->ulRrcEstCause = ucRrcCause;

    pSndMsg->usPdpContextStatus = usPdpContextStatus;

    #if(PS_UE_REL_VER >= PS_PTL_VER_R7)
    pSndMsg->usUplinkDataStatus = usUplinkDataStatus;
    #endif

    pSndMsg->MsgHeader.ulSenderCpuId = VOS_LOCAL_CPUID;
    pSndMsg->MsgHeader.ulSenderPid = WUEPS_PID_RABM;
    pSndMsg->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pSndMsg->MsgHeader.ulReceiverPid = WUEPS_PID_GMM;
    pSndMsg->MsgHeader.ulMsgName = ID_RABM_GMM_REESTABLISH_REQ;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_RABM, pSndMsg))
    {
        NAS_WARNING_LOG(WUEPS_PID_RABM, "RABM_SndRabReestReq():WARNING:SEND MSG FIAL");
    }

    return;
}
VOS_VOID RABM_SndPdcpDataReq(
                         VOS_UINT8  ucEntId,
                         TAFRABM_PS_DATA_REQ_STRU  *pData
                         )
{
    VOS_UINT32     ulDataLen;
    VOS_UINT8      ucRbId;

    ucRbId = g_aRabmPsEnt[ucEntId].RabInfo.aucRbId[0];                          /* 获得RB ID                                */

    ulDataLen = TTF_MemGetLen(WUEPS_PID_RABM, pData->pData);

    if ( 0 == ulDataLen )
    {
        return;
    }

    NAS_UTRANCTRL_RABM_AdaptPdcpDataReq(ucRbId, pData->pData, ulDataLen * 8);
    return;
}
VOS_VOID RABM_SndRrcQosUpdReq(
    VOS_UINT8                           ucEntId                                 /* RABM实体ID                               */
)
{
    QOS_INFO_STRU  *pQos = RABM_POINTER_NULL;

    pQos = (QOS_INFO_STRU *)RABM_Malloc(sizeof(QOS_INFO_STRU));
    if (VOS_NULL_PTR == pQos)
    {
        PS_LOG(WUEPS_PID_RABM, VOS_NULL, PS_PRINT_ERROR, "RABM_SndRrcQosUpdReq:ERROR:Alloc mem fail!");
        return;
    }

    RABM_Memset((VOS_VOID *)pQos,0,sizeof(QOS_INFO_STRU));

    pQos->ulRabCnt = 1;                                                         /* 设置RAB 的个数                           */
    pQos->aRab[0].ulRabId = g_aRabmPsEnt[ucEntId].RabInfo.ucRabId;              /* 设置RAB ID                               */
    pQos->aRab[0].ulPppValid = g_aRabmPsEnt[ucEntId].ucPppFlg;                  /* 设置PPP标志                              */
    pQos->aRab[0].ulDeliveryErrSdu = g_aRabmPsEnt[ucEntId].QoS.aucQosValue[3] & NAS_RABM_QOS_DELIVERY_ERRORNEOUS_SDU_MASK; /* 设置Error SDU Delivery                   */
    pQos->aRab[0].ulCnDomainId = RABM_CN_PS;

    if(AS_SUCCESS != g_RabmRrcImportFunc.RrRabmRabQosUpdate(pQos))
    {
        PS_LOG(WUEPS_PID_RABM, VOS_NULL, PS_PRINT_WARNING, "RABM_SndRrcQosUpdReq:WARNING:g_RabmRrcImportFunc.RrRabmRabQosUpdate execute fail!");
    }
    RABM_Free(pQos);
    return;
}
VOS_VOID RABM_SndTafPsDataInd(
                          VOS_UINT8                  ucEntId,                       /* RAB ID所对应的NSAPI索引                  */
                          PDCP_RABM_DATA_BUF_STRU  *pRcvMsg                       /* 数据指针                                 */
                          )
{
    ST_APP_RABM_DATA  PsDataInd;                                                /* 定义局部结构体变量                       */

    PsDataInd.ucDomain  = D_RABM_PS_DOMAIN;                                     /* 填写域值                                 */
    PsDataInd.ucRabId   = (VOS_UINT8)(ucEntId + RABM_NSAPI_OFFSET);                 /* 保存RAB ID所对应的NSAPI                  */
    PsDataInd.ulDataLen = pRcvMsg->ulDataLen;                                   /* 保存数据长度                             */
    PsDataInd.pucData   = pRcvMsg->pucData;                                     /* 保存数据                                 */

    Api_AppRabmDataInd( &PsDataInd );
}
VOS_VOID RABM_SndRrcRabRsp(
                       VOS_UINT8  ucRabNum,                                         /* 应答消息中包含的RB个数                   */
                       RRRABM_RAB_IND_STRU  *pMsg                               /* 目前处理的消息                           */
                       )
{
    RAB_RSP_INFO_STRU   *pSndMsg = RABM_POINTER_NULL;
    VOS_UINT8               i;
    VOS_UINT8               ucEntId;
    VOS_UINT32               ulResult;
    VOS_UINT8               ucErrorSdu;

    ulResult = AS_RAB_RSP_OK;
    if( 0 == ucRabNum )
    {
        ulResult = AS_RAB_RSP_NG;
    }

    pSndMsg = (RAB_RSP_INFO_STRU *)RABM_Malloc(sizeof(RAB_RSP_INFO_STRU));
    if (VOS_NULL_PTR == pSndMsg)
    {
        PS_LOG(WUEPS_PID_RABM, VOS_NULL, PS_PRINT_ERROR, "RABM_SndRrcRabRsp:ERROR:Alloc msg fail!");
        return;
    }

    RABM_Memset((VOS_VOID *)pSndMsg,0,sizeof(RAB_RSP_INFO_STRU));

    pSndMsg->ulRabCnt = ucRabNum;
    for(i = 0; i< pSndMsg->ulRabCnt; i++)
    {
        if(RABM_CN_PS == pMsg->aRabInfo[i].ulCnDomainId)
        {
            pSndMsg->aRabInfo[i].ulCnDomainId  = RABM_CN_PS;
            pSndMsg->aRabInfo[i].ulRabId = pMsg->aRabInfo[i].ulRabId;
            ucEntId = (VOS_UINT8)((pMsg->aRabInfo[i].ulRabId & 0x0f)
                - RABM_NSAPI_OFFSET);
            if(RABM_TC_STATE_NULL == g_aRabmPsEnt[ucEntId].ucState)
            {                                                                   /* TC已经启动                               */
                pSndMsg->aRabInfo[i].ulDeliveryErrSdu =
                    RABM_ERROR_SDU_DELIVERY_NO_DET;
                pSndMsg->aRabInfo[i].ulPppValid = WUEPS_FALSE;
            }
            else
            {
                pSndMsg->aRabInfo[i].ulPppValid = g_aRabmPsEnt[ucEntId].ucPppFlg; /* 赋值ppp标志                              */
                ucErrorSdu = g_aRabmPsEnt[ucEntId].QoS.aucQosValue[3] & NAS_RABM_QOS_DELIVERY_ERRORNEOUS_SDU_MASK;     /* 获得error sdu delivery信息               */
                if((0 == ucErrorSdu) || (0x07 == ucErrorSdu))
                {
                    pSndMsg->aRabInfo[i].ulDeliveryErrSdu =
                        RABM_ERROR_SDU_DELIVERY_NO_DET;                         /* 赋值error sdu delivery信息               */
                }
                else
                {
                    pSndMsg->aRabInfo[i].ulDeliveryErrSdu = ucErrorSdu;
                }
            }
        }
        else
        {
            pSndMsg->aRabInfo[i].ulCnDomainId  = RABM_CN_CS;
            pSndMsg->aRabInfo[i].ulRabId = pMsg->aRabInfo[i].ulRabId;
            pSndMsg->aRabInfo[i].ulDeliveryErrSdu =
                RABM_ERROR_SDU_DELIVERY_NO_DET;
            pSndMsg->aRabInfo[i].ulPppValid = WUEPS_TRUE;
        }
    }
    pSndMsg->ulOpId = pMsg->ulOpId;

    if(AS_SUCCESS != g_RabmRrcImportFunc.RrRabmRabInfoRsp(ulResult,pSndMsg))
    {
        PS_LOG(WUEPS_PID_RABM, VOS_NULL, PS_PRINT_WARNING, "RABM_SndRrcRabRsp:WARNING:g_RabmRrcImportFunc.RrRabmRabInfoRsp execute fail!");
    }
    RABM_Free(pSndMsg);
    return;
}

/*******************************************************************************
  Module:   RABM_SndSmDeactReq
  Function: 通知SM相应的NSAPI的RAB已经释放
  Input:    VOS_UINT8   ucNsapi           Network service access point identifier
  Output:   VOS_VOID
  NOTE:
  Return:   VOS_VOID
  History:
      1. 张志勇      2003.12.11   新规作成
      2.  日    期   : 2006年10月21日
          作    者   : luojian id:60022475
          修改内容   : 调用PS_ALLOC_MSG申请消息包时传入的长度应减去VOS
          消息头的长度，问题单号:A32D06583
*******************************************************************************/
VOS_VOID RABM_SndSmDeactReq(
                        VOS_UINT8  ucNsapi                                          /* Network service access point identifier  */
                        )
{
    RABMSM_DEACTIVATE_REQ_STRU *pSndMsg = RABM_POINTER_NULL;

    pSndMsg = (RABMSM_DEACTIVATE_REQ_STRU *)
                PS_ALLOC_MSG(WUEPS_PID_RABM, sizeof(RABMSM_DEACTIVATE_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pSndMsg)
    {
        PS_LOG(WUEPS_PID_RABM, VOS_NULL, PS_PRINT_ERROR, "RABM_SndSmDeactReq:ERROR:Alloc msg fail!");
        return;
    }

    pSndMsg->ulNsapi = ucNsapi;

    pSndMsg->MsgHeader.ulSenderCpuId = VOS_LOCAL_CPUID;
    pSndMsg->MsgHeader.ulSenderPid = WUEPS_PID_RABM;
    pSndMsg->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pSndMsg->MsgHeader.ulReceiverPid = WUEPS_PID_SM;
    pSndMsg->MsgHeader.ulMsgName = RABMSM_DEACTIVATE_REQ;

    PS_SEND_MSG(WUEPS_PID_RABM, pSndMsg);

    return;
}

/*******************************************************************************
  Module:   RABM_SndCcSyncInd
  Function: 通知CC RAB已经建立或释放
  Input:    VOS_UINT8   ucResult           结果:建立或释放
            VOS_UINT32   ulSi               建立或释放rab的stream id
  Output:   VOS_VOID
  NOTE:
  Return:   VOS_VOID
  History:
      1. 张志勇      2003.12.11   新规作成
      2. 日    期   : 2006年10月21日
         作    者   : luojian id:60022475
         修改内容   : 调用PS_ALLOC_MSG申请消息包时传入的长度应减去VOS
         消息头的长度，问题单号:A32D06583
*******************************************************************************/
VOS_VOID RABM_SndCcSyncInd(
                       VOS_UINT8  ucResult,                                         /* 结果:建立或释放                          */
                       VOS_UINT32  ulSi                                              /* 建立或释放rab的stream id                 */
                       )
{
    RABMCC_SYNC_IND_STRU *pSndMsg = RABM_POINTER_NULL;

    pSndMsg = ( RABMCC_SYNC_IND_STRU * )
                PS_ALLOC_MSG(WUEPS_PID_RABM, sizeof(RABMCC_SYNC_IND_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pSndMsg)
    {
        PS_LOG(WUEPS_PID_RABM, VOS_NULL, PS_PRINT_ERROR, "RABM_SndCcSyncInd:ERROR:Alloc msg fail!");
        return;
    }

    pSndMsg->ulSi = ulSi;
    pSndMsg->ulResult = ucResult;

    pSndMsg->MsgHeader.ulSenderCpuId = VOS_LOCAL_CPUID;
    pSndMsg->MsgHeader.ulSenderPid = WUEPS_PID_RABM;
    pSndMsg->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pSndMsg->MsgHeader.ulReceiverPid = WUEPS_PID_CC;
    pSndMsg->MsgHeader.ulMsgName = RABMCC_SYNC_IND;

    PS_SEND_MSG(WUEPS_PID_RABM, pSndMsg);
    return;
}



VOS_VOID RABM_SndCallSyncInd(
    VOS_UINT32                          ulResult,
    RAB_INFO_STRU                       *pRabInfo
)
{
    RABM_CALL_SYNC_IND_STRU *pSndMsg = RABM_POINTER_NULL;
    VOS_UINT8               ucLoop;

    pSndMsg = ( RABM_CALL_SYNC_IND_STRU * )
                PS_ALLOC_MSG(WUEPS_PID_RABM, sizeof(RABM_CALL_SYNC_IND_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pSndMsg)
    {
        PS_LOG(WUEPS_PID_RABM, VOS_NULL, PS_PRINT_ERROR, "RABM_SndCallSyncInd:ERROR:Alloc msg fail!");
        return;
    }

    pSndMsg->MsgHeader.ulSenderCpuId = VOS_LOCAL_CPUID;
    pSndMsg->MsgHeader.ulSenderPid = WUEPS_PID_RABM;
    pSndMsg->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pSndMsg->MsgHeader.ulReceiverPid = WUEPS_PID_TAF;
    pSndMsg->MsgHeader.ulMsgName = RABM_CALL_SYNC_IND;

    /* 填写当前的Rab状态，RabId和RbId */
    pSndMsg->ulRabState = ulResult;
    pSndMsg->ulRabId    = pRabInfo->ulRabId;
    if (pRabInfo->ulRbNum <= RABM_CALL_MAX_RB_NUM_PER_RAB)
    {
        pSndMsg->ulRbNum = pRabInfo->ulRbNum;
    }
    else
    {
        pSndMsg->ulRbNum = RABM_CALL_MAX_RB_NUM_PER_RAB;
    }

    /* 当前的RbId可能有多个，所以目前的处理时都带上 */
    for(ucLoop = 0; ucLoop < pSndMsg->ulRbNum; ucLoop++)
    {
        pSndMsg->aulRbId[ucLoop] = pRabInfo->aulRbId[ucLoop];
    }


    PS_SEND_MSG(WUEPS_PID_RABM, pSndMsg);
    return;
}



VOS_VOID RABM_SndTafCsDataInd(
                          RLC_RABM_CS_DATA_IND_STRU      *pRcvMsg,                   /* 数据指针                                 */
                          VOS_UINT8                     ucEntId                     /* CS域RABM实体索引                         */
                          )
{
    ST_APP_RABM_DATA   CsDataInd;                                               /* 定义局部结构体变量                       */

    CsDataInd.ucDomain  = D_RABM_CS_DOMAIN;                                     /* 填写域值                                 */
    CsDataInd.ucRabId   = g_aRabmCsEnt[ucEntId].ucSi;                           /* 保存RAB ID所对应的SI                     */
    CsDataInd.ulDataLen = (VOS_UINT32)(pRcvMsg->usDataBitLen);                  /* 保存数据长度(bit长度)                    */
    CsDataInd.pucData   = pRcvMsg->pucData;                                     /* 保存数据                                 */

    Api_AppRabmDataInd( &CsDataInd );                                           /* 向APP发送CS域数据                        */
}

/*******************************************************************************
  Module:   RABM_SndTcTestCnf
  Function: 通知tc已经激活/去激活测试环
  Input:    TCRABM_TEST_CNF_STRU *pSndMsg
  Output:   VOS_VOID
  NOTE:
  Return:   VOS_VOID
  History:
      1.   张志勇      2004.05.09   新规作成
*******************************************************************************/
VOS_VOID RABM_SndTcTestCnf(
                       TCRABM_TEST_CNF_STRU  *pSndMsg
                       )
{
    pSndMsg->MsgHeader.ulSenderCpuId = VOS_LOCAL_CPUID;
    pSndMsg->MsgHeader.ulSenderPid = WUEPS_PID_RABM;
    pSndMsg->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pSndMsg->MsgHeader.ulReceiverPid = WUEPS_PID_TC;
    pSndMsg->MsgHeader.ulMsgName = TCRABM_TEST_CNF;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_RABM, pSndMsg))
    {
        NAS_WARNING_LOG(WUEPS_PID_RABM, "RABM_SndTcTestCnf():WARNING:SEND MSG FIAL");
    }

    return;
}


VOS_VOID RABM_SndTcTestLoopCnf()
{
    TCRABM_TEST_LOOP_CNF_STRU  *pSndMsg;

    pSndMsg = (TCRABM_TEST_LOOP_CNF_STRU *)
                PS_ALLOC_MSG(WUEPS_PID_RABM, sizeof(TCRABM_TEST_LOOP_CNF_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pSndMsg)
    {
        PS_LOG(WUEPS_PID_RABM, VOS_NULL, PS_PRINT_ERROR, "RABM_SndTcTestLoopCnf:ERROR:Alloc msg fail!");
        return;
    }

    pSndMsg->MsgHeader.ulSenderCpuId = VOS_LOCAL_CPUID;
    pSndMsg->MsgHeader.ulSenderPid = WUEPS_PID_RABM;
    pSndMsg->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pSndMsg->MsgHeader.ulReceiverPid = WUEPS_PID_TC;
    pSndMsg->MsgHeader.ulMsgName = TCRABM_TEST_LOOP_CNF;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_RABM, pSndMsg))
    {
        NAS_WARNING_LOG(WUEPS_PID_RABM, "RABM_SndTcTestLoopCnf():WARNING:SEND MSG FIAL");
    }
    return;
}


VOS_VOID RABM_SndTcPsData_Mode_I_Handling(
    VOS_UINT8                           ucEntId,
    PDCP_RABM_TC_DATA_STRU             *pData
)
{
    TTF_MEM_ST                         *pstDataMem;
    VOS_UINT8                          *pucData;
    VOS_UINT8                           ucSpare;
    VOS_UINT32                          i;
    VOS_UINT8                           ucData;
    VOS_UINT16                          usSuffix;
    VOS_UINT32                          ulDataAddLen;
    VOS_UINT32                          ulDataBitLen;
    VOS_UINT16                          usMaxSduSize;
    VOS_UINT8                           ucGcfFlg;

    ulDataAddLen = 0;
    ulDataBitLen = pData->ulDataBitLen;

    if (VOS_FALSE == g_RabmTcInf.aRbInf[ucEntId].bRlcSduSizeValid)
    {
        /* 表示上行数据长度无效，直接返回，可以打印异常 */
        NAS_WARNING_LOG(WUEPS_PID_RABM, "RABM_SndTcPsData_Mode_I_Handling:WARNING:ulRlcSduSize is invalid!");
        return;
    }

    ucGcfFlg = ((NAS_UTRANCTRL_UTRAN_MODE_TDD == NAS_UTRANCTRL_GetCurrUtranMode())
                && (g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize == g_ulNasRabmStubRlcSduSize)) ? VOS_TRUE : VOS_FALSE;

    if ((0 == g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize) || (VOS_TRUE == ucGcfFlg))
    {
        usMaxSduSize = (VOS_UINT16)((ulDataBitLen + 7) / 8);

        pstDataMem = TTF_MemBlkAlloc(WUEPS_PID_RABM,
                                TTF_MEM_POOL_ID_UL_RLC_SDU,
                                usMaxSduSize);
        if (VOS_NULL_PTR == pstDataMem)
        {
            NAS_ERROR_LOG(WUEPS_PID_RABM, "RABM_SndTcPsData_Mode_I_Handling:ERROR:TcTest pSendPsData Error!");
            return;
        }

        pucData = pstDataMem->pData;

        /* 上下行数据长度相同 */
        TTF_MemGet(WUEPS_PID_RABM, pData->pstDataMem, 0, pucData, usMaxSduSize);

        /* 更新used长度 */
        pstDataMem->usUsed  = usMaxSduSize;

        NAS_UTRANCTRL_RABM_AdaptPdcpDataReq(pData->ucRbId, pstDataMem, ulDataBitLen);

    }
    else
    {
        usMaxSduSize = (VOS_UINT16)((g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize + 7) / 8);

        pstDataMem = TTF_MemBlkAlloc(WUEPS_PID_RABM,
                                 TTF_MEM_POOL_ID_UL_RLC_SDU,
                                 usMaxSduSize);
        if (VOS_NULL_PTR == pstDataMem)
        {
            NAS_ERROR_LOG(WUEPS_PID_RABM, "RABM_SndTcPsData_Mode_I_Handling:ERROR:TcTest pSendPsData Error!");
            return;
        }

        pucData = pstDataMem->pData;

        if (ulDataBitLen == g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize)
        {
            /* 复制g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize长度的数据 */
            TTF_MemGet(WUEPS_PID_RABM, pData->pstDataMem, 0,
                       pucData, usMaxSduSize);
        }
        else if (ulDataBitLen > g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize)
        {
            /* 复制g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize长度的数据 */
            TTF_MemGet(WUEPS_PID_RABM, pData->pstDataMem, 0,
                       pucData, usMaxSduSize);
        }
        else
        {
            /* 使用PDCP上报的数据循环填充至g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize长度 */
            ucSpare = (VOS_UINT8)(ulDataBitLen % 8);                            /* 获得bit值                                */
            if (0 == ucSpare)
            {
                for (i = 1; (ulDataBitLen * i) <= g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize; i++)
                {                                                               /* 组合上行数据                             */
                    if (1 == i)
                    {
                        TTF_MemGet(WUEPS_PID_RABM,
                            pData->pstDataMem,
                            0,
                            pucData + (((ulDataBitLen + 7) / 8) * (i - 1)),
                            (VOS_UINT16)((ulDataBitLen + 7) / 8) );
                    }
                    else
                    {
                        DRV_RT_MEMCPY(pucData + (((ulDataBitLen + 7) / 8) * (i - 1)),
                                    pucData,
                                    (VOS_UINT16)((ulDataBitLen + 7) / 8));
                    }

                    ulDataAddLen += ulDataBitLen;
                }

                if (ulDataAddLen < g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize)
                {                                                               /* 已经合成的数据长度小于上行数据长度       */
                    DRV_RT_MEMCPY(pucData + ((ulDataAddLen + 7) / 8),
                                pucData,
                                (VOS_UINT16)(((g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize - ulDataAddLen) + 7) / 8));
                }
            }
            else
            {
                TTF_MemGet(WUEPS_PID_RABM,
                    pData->pstDataMem,
                    0,
                    pucData,
                    (VOS_UINT16)(ulDataBitLen / 8) );

                usSuffix    = (VOS_UINT16)(((ulDataBitLen + 7) / 8) - 1);       /* 获得数据的下标                           */
                ucData      = pucData[usSuffix];
                ucData     |= (pucData[0] >> ucSpare);                          /* 组合数据                                 */
                ulDataAddLen   = ulDataBitLen;

                for (i = ulDataBitLen;i < g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize; )
                {
                    *((pucData + ((ulDataAddLen + 7) / 8)) -1) = ucData;        /* 组合上行数据                             */
                    usSuffix = (VOS_UINT16)(((ulDataAddLen % ulDataBitLen) + 7) / 8);
                    ucData = (VOS_UINT8)((pucData[usSuffix]) << (8 - ucSpare));
                    if (usSuffix == (((ulDataBitLen + 7) - 8) / 8))
                    {                                                           /* 已经到达收到数据的末尾                   */
                        ulDataAddLen = ulDataBitLen;                            /* 重新初始化数据长度                       */
                        usSuffix = (VOS_UINT16)(((ulDataAddLen % ulDataBitLen)
                                    + 7) / 8);
                    }
                    else
                    {
                        usSuffix = (VOS_UINT16)((((ulDataAddLen % ulDataBitLen)
                                    + 7) / 8) +1);
                    }

                    ucData |= pucData[usSuffix] >> ucSpare;
                    i += 8;
                    ulDataAddLen += 8;
                }
            }
        }

        ucSpare = (VOS_UINT8)(g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize % 8);

        if (0 != ucSpare)
        {
            pucData[((g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize + 7) / 8)-1]
                &= (VOS_INT8)(0xff << (8-ucSpare));
        }

        /* 更新used长度 */
        pstDataMem->usUsed = usMaxSduSize;

        NAS_UTRANCTRL_RABM_AdaptPdcpDataReq(pData->ucRbId,
                                            pstDataMem,
                                            g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize);
    }

    return;
}
VOS_VOID RABM_SndTcPsData_Mode_II_Handling(
    VOS_UINT8                           ucEntId,
    PDCP_RABM_TC_DATA_STRU             *pData
)
{
    TTF_MEM_ST                         *pstDataMem;
    VOS_UINT8                          *pucData;
    VOS_UINT32                          ulDataBitLen;
    VOS_UINT32                          ulMaxDataLen;
    VOS_UINT32                          ulCrc;
    VOS_UINT32                          ulCrcTmp;
    VOS_UINT16                          usCrcTmp;
    VOS_UINT8                           ucSpare;
    VOS_UINT16                          usDataIndex;


    ulDataBitLen = 0;


    /*获取适当的上行环回报文长度，单位:Bit*/
    NAS_UTRANCTRL_RABM_GetRbSuitableUlPduSize( pData->ucRbId,
                                              pData->ulDataBitLen,
                                              pData->stCrcInfo.ucCrcBitLen,
                                              &ulDataBitLen );

    if ( ( pData->stCrcInfo.ucCrcBitLen != 0 ) && ( pData->stCrcInfo.ucCrcBitLen != 8 )
      && ( pData->stCrcInfo.ucCrcBitLen != 12 ) && ( pData->stCrcInfo.ucCrcBitLen != 16 )
      && ( pData->stCrcInfo.ucCrcBitLen != 24 ) )
    {
        RABM_LOG_WARNING( "\n RABM_SndTcPsData:Crc Length Error! \r" );

        pstDataMem = TTF_MemBlkAlloc(WUEPS_PID_RABM,
                                TTF_MEM_POOL_ID_UL_RLC_SDU,
                                (VOS_UINT16)((ulDataBitLen + 7) / 8));
        if (VOS_NULL_PTR == pstDataMem)
        {
            NAS_ERROR_LOG(WUEPS_PID_RABM, "RABM_SndTcPsData_Mode_II_Handling:ERROR:TcTest pSendPsData Error!");
            return;
        }

        pucData = pstDataMem->pData;

        if (pData->ulDataBitLen <= ulDataBitLen)
        {
            TTF_MemGet(WUEPS_PID_RABM,
                pData->pstDataMem,
                0,
                pucData,
                (VOS_UINT16)((pData->ulDataBitLen + 7) / 8));
        }
        else
        {
            TTF_MemGet(WUEPS_PID_RABM,
                pData->pstDataMem,
                0,
                pucData,
                (VOS_UINT16)((ulDataBitLen + 7) / 8));
        }

        NAS_UTRANCTRL_RABM_AdaptPdcpDataReq(pData->ucRbId,
                                           pstDataMem,
                                           ulDataBitLen);

    }
    else if ( pData->stCrcInfo.ucCrcBitLen == 0 )
    {

        pstDataMem = TTF_MemBlkAlloc(WUEPS_PID_RABM,
                                TTF_MEM_POOL_ID_UL_RLC_SDU,
                                (VOS_UINT16)((ulDataBitLen + 7) / 8));
        if (VOS_NULL_PTR == pstDataMem)
        {
            NAS_ERROR_LOG(WUEPS_PID_RABM, "RABM_SndTcPsData_Mode_II_Handling:ERROR:TcTest pSendPsData Error!");
            return;
        }

        pucData = pstDataMem->pData;

        if (pData->ulDataBitLen <= ulDataBitLen)
        {
            TTF_MemGet(WUEPS_PID_RABM,
                pData->pstDataMem,
                0,
                pucData,
                (VOS_UINT16)((pData->ulDataBitLen + 7) / 8));
        }
        else
        {
            TTF_MemGet(WUEPS_PID_RABM,
                pData->pstDataMem,
                0,
                pucData,
                (VOS_UINT16)((ulDataBitLen + 7) / 8));
        }

        NAS_UTRANCTRL_RABM_AdaptPdcpDataReq(pData->ucRbId,
                                           pstDataMem,
                                           ulDataBitLen);

    }
    else
    {
        if ( (VOS_UINT32)( pData->ulDataBitLen + pData->stCrcInfo.ucCrcBitLen ) <= ulDataBitLen )
        {
            ulMaxDataLen = ulDataBitLen;
        }
        else
        {
            ulMaxDataLen = pData->ulDataBitLen + pData->stCrcInfo.ucCrcBitLen;
        }

        pstDataMem = TTF_MemBlkAlloc(WUEPS_PID_RABM,
                                TTF_MEM_POOL_ID_UL_RLC_SDU,
                                (VOS_UINT16)((ulMaxDataLen + 7) / 8));
        if (VOS_NULL_PTR == pstDataMem)
        {
            NAS_ERROR_LOG(WUEPS_PID_RABM, "RABM_SndTcPsData_Mode_II_Handling:ERROR:TcTest pSendPsData Error!");
            return;
        }

        pucData = pstDataMem->pData;

        PS_NAS_MEM_SET(pucData, 0, (ulMaxDataLen + 7) / 8);

        TTF_MemGet(WUEPS_PID_RABM,
            pData->pstDataMem,
            0,
            pucData,
            (VOS_UINT16)((pData->ulDataBitLen + 7) / 8));

        ucSpare = (VOS_UINT8)(pData->ulDataBitLen % 8);

        /*获取CRC*/
        ulCrc = 0;
        switch(pData->stCrcInfo.ucCrcBitLen)
        {
            case 0:
                ulCrc = 0;
                break;
            case 8:
                ulCrc = pData->stCrcInfo.aucCrc[0];
                break;
            case 12:
                ulCrc |= ((VOS_UINT32)pData->stCrcInfo.aucCrc[0] << 4) & 0x0FF0;
                ulCrc |= ((VOS_UINT32)pData->stCrcInfo.aucCrc[1] >> 4) & 0x000F;
                break;
            case 16:
                ulCrc |= ((VOS_UINT32)pData->stCrcInfo.aucCrc[0] << 8) & 0xFF00;
                ulCrc |= (VOS_UINT32)pData->stCrcInfo.aucCrc[1];
                break;
            case 24:
                ulCrc |= (VOS_UINT32)pData->stCrcInfo.aucCrc[0] << 16;
                ulCrc |= (VOS_UINT32)pData->stCrcInfo.aucCrc[1] << 8;
                ulCrc |= (VOS_UINT32)pData->stCrcInfo.aucCrc[2];
                break;
            default:
                break;
        }

        if ( 0 == ucSpare )
        {
            usDataIndex = (VOS_UINT16)(pData->ulDataBitLen / 8);
            switch ( pData->stCrcInfo.ucCrcBitLen )
            {
                case 8:
                    pucData[usDataIndex] = (VOS_UINT8)ulCrc;
                    break;
                case 12:
                    pucData[usDataIndex] = (VOS_UINT8)( ulCrc >> 4 );
                    pucData[usDataIndex + 1] = (VOS_UINT8)( ulCrc << 4 );
                    break;
                case 16:
                    pucData[usDataIndex] = (VOS_UINT8)( ulCrc >> 8 );
                    pucData[usDataIndex + 1] = (VOS_UINT8)ulCrc;
                    break;
                case 24:
                    pucData[usDataIndex] = (VOS_UINT8)( ulCrc >> 16 );
                    pucData[usDataIndex + 1] = (VOS_UINT8)( ulCrc >> 8 );
                    pucData[usDataIndex + 2] = (VOS_UINT8)ulCrc;
                    break;
                default:
                    break;
            }
        }
        else
        {
            usDataIndex = (VOS_UINT16)(pData->ulDataBitLen / 8);
            switch ( pData->stCrcInfo.ucCrcBitLen )
            {
                case 8:
                    ulCrc &= 0x000000ff;
                    usCrcTmp = (VOS_UINT16)( ( ulCrc << ( 8 - ucSpare ) ) );
                    pucData[usDataIndex] &= (VOS_UINT8)(0Xff << ( 8 - ucSpare ));
                    pucData[usDataIndex] |= (VOS_UINT8)( usCrcTmp >> 8 );
                    pucData[usDataIndex + 1] = (VOS_UINT8)( usCrcTmp );
                    break;
                case 12:
                    ulCrc &= 0x00000fff;
                    if ( ucSpare <= 4 )
                    {
                        usCrcTmp = (VOS_UINT16)( ulCrc << ( 4 - ucSpare ) );
                        pucData[usDataIndex] &= (VOS_UINT8)(0xff << ( 8 - ucSpare ));
                        pucData[usDataIndex] |= (VOS_UINT8)( usCrcTmp >> 8 );
                        pucData[usDataIndex + 1] = (VOS_UINT8)( usCrcTmp );
                    }
                    else
                    {
                        ulCrcTmp = ulCrc << ( 12 - ucSpare );
                        pucData[usDataIndex] &= (VOS_UINT8)(0xff << ( 8 - ucSpare ));
                        pucData[usDataIndex] |= (VOS_UINT8)( ulCrcTmp >> 16 );
                        pucData[usDataIndex + 1] = (VOS_UINT8)( ulCrcTmp >> 8 );
                        pucData[usDataIndex + 2] = (VOS_UINT8)( ulCrcTmp );
                    }
                    break;
                case 16:
                    ulCrc &= 0x0000ffff;
                    ulCrcTmp = ulCrc << ( 8 - ucSpare );
                    pucData[usDataIndex] &= (VOS_UINT8)(0xff << ( 8 - ucSpare ));
                    pucData[usDataIndex] |= (VOS_UINT8)( ulCrcTmp >> 16 );
                    pucData[usDataIndex + 1] = (VOS_UINT8)( ulCrcTmp >> 8 );
                    pucData[usDataIndex + 2] = (VOS_UINT8)( ulCrcTmp );
                    break;
                case 24:
                    ulCrc &= 0x00ffffff;
                    ulCrcTmp = ulCrc << ( 8 - ucSpare );
                    pucData[usDataIndex] &= (VOS_UINT8)(0xff << ( 8 - ucSpare ));
                    pucData[usDataIndex] |= (VOS_UINT8)( ulCrcTmp >> 24 );
                    pucData[usDataIndex + 1] = (VOS_UINT8)( ulCrcTmp >> 16 );
                    pucData[usDataIndex + 2] = (VOS_UINT8)( ulCrcTmp >> 8 );
                    pucData[usDataIndex + 3] = (VOS_UINT8)( ulCrcTmp );
                    break;
                default:
                    break;
            }
        }

        NAS_UTRANCTRL_RABM_AdaptPdcpDataReq(pData->ucRbId,
                                           pstDataMem,
                                           ulDataBitLen);
    }

    return;
}
VOS_VOID RABM_SndTcPsData(
                      VOS_UINT8                 ucEntId,
                      PDCP_RABM_TC_DATA_STRU    *pData
                      )
{
    if ( TC_MODE_1 == g_RabmTcInf.ucTestLoopMode )
    {
        RABM_SndTcPsData_Mode_I_Handling(ucEntId, pData);
    }
    else if ( TC_MODE_2 == g_RabmTcInf.ucTestLoopMode )
    {
        RABM_SndTcPsData_Mode_II_Handling(ucEntId, pData);
    }
    else
    {
        PS_LOG1(WUEPS_PID_RABM, VOS_NULL,  PS_PRINT_ERROR, "RABM_SndTcPsData:ERROR:ucTestLoopMode Error!", g_RabmTcInf.ucTestLoopMode);
        return;
    }

    return;
}


VOS_UINT32  Rabm_CrcCalculate( VOS_UCHAR *pTransferBlock,
                                       VOS_UINT32 TbSize, VOS_UINT8 CrcLen)
{
    VOS_UINT16  usLoop;
    VOS_UINT32  ulPoly,ulCrc = 0;
    VOS_UINT8   ucCrc_8;
    VOS_UINT16  usCrc_12,usCrc_16;
    VOS_UINT32  ulCrc_24;
    VOS_UCHAR   *pucData, ucCycle;

    switch( CrcLen )
    {
        case 0:
            ulPoly = 0x0;
            return ulCrc;
        case 8:
            ulPoly  = 0xd9;
            ucCrc_8 = 0;
            pucData = pTransferBlock;
            ucCycle = 0x80;
            for ( usLoop = 0; usLoop < TbSize; usLoop++ )
            {
                if ( ( usLoop != 0 ) && ( (usLoop % 8) == 0 ) )
                {
                    ucCycle = 0x80;
                    pucData++;
                }

                if ( ( ucCrc_8 & 0x01 ) == 1 )
                {
                    ucCrc_8 >>= 1;
                    ucCrc_8 ^= (VOS_UINT8)ulPoly;
                }
                else
                {
                    ucCrc_8 >>= 1;
                }

                if( ( *pucData & ucCycle ) != 0 )
                {
                    ucCrc_8 ^= (VOS_UINT8)ulPoly;
                }

                ucCycle >>= 1;
            }
            ulCrc = ( VOS_UINT32 )ucCrc_8;
            break;
        case 12:
            ulPoly   = 0xf01;
            usCrc_12 = 0;
            pucData  = pTransferBlock;
            ucCycle  = 0x80;
            for ( usLoop = 0; usLoop < TbSize; usLoop++ )
            {
                if ( ( usLoop != 0 ) && ( (usLoop % 8) == 0 ) )
                {
                    ucCycle = 0x80;
                    pucData++;
                }

                if ( ( usCrc_12 & 0x0001 ) == 1 )
                {
                    usCrc_12 >>= 1;
                    usCrc_12 ^= (VOS_UINT16)ulPoly;
                }
                else
                {
                    usCrc_12 >>= 1;
                }

                if( ( *pucData & ucCycle ) != 0 )
                {
                    usCrc_12 ^= (VOS_UINT16)ulPoly;
                }

                ucCycle >>= 1;
            }
            ulCrc = ( VOS_UINT32 )usCrc_12;
            break;
        case 16:
            ulPoly   = 0x8408;
            usCrc_16 = 0;
            pucData  = pTransferBlock;
            ucCycle  = 0x80;
            for ( usLoop = 0; usLoop < TbSize; usLoop++ )
            {
                if ( ( usLoop != 0 ) && ( (usLoop % 8) == 0 ) )
                {
                    ucCycle = 0x80;
                    pucData++;
                }

                if ( ( usCrc_16 & 0x0001 ) == 1 )
                {
                    usCrc_16 >>= 1;
                    usCrc_16 ^= (VOS_UINT16)ulPoly;
                }
                else
                {
                    usCrc_16 >>= 1;
                }

                if( ( *pucData & ucCycle ) != 0 )
                {
                    usCrc_16 ^= (VOS_UINT16)ulPoly;
                }

                ucCycle >>= 1;
            }
            ulCrc = ( VOS_UINT32 )usCrc_16;
            break;
        case 24:
            ulPoly   = 0xc60001;
            ulCrc_24 = 0;
            pucData  = pTransferBlock;
            ucCycle  = 0x80;
            for ( usLoop = 0; usLoop < TbSize; usLoop++ )
            {
                if ( ( usLoop != 0 ) && ( (usLoop % 8) == 0 ) )
                {
                    ucCycle = 0x80;
                    pucData++;
                }

                if ( ( ulCrc_24 & 0x00000001 ) == 1 )
                {
                    ulCrc_24 >>= 1;
                    ulCrc_24 ^= ulPoly;
                }
                else
                {
                    ulCrc_24 >>= 1;
                }

                if( ( *pucData & ucCycle ) != 0 )
                {
                    ulCrc_24 ^= ulPoly;
                }

                ucCycle >>= 1;
            }
            ulCrc = ( VOS_UINT32 )ulCrc_24;
            break;
        default:
            break;
    }
    return ulCrc;
}

/*******************************************************************************
  Module:   RABM_SndTcCsData_Mode_I_Handling
  Function: RABM_SndTcCsData 将复杂度: TC_MODE_1处理
  Input:    VOS_UINT8                       ucEntId
            RABMRLC_CS_DATA_IND_STRU    *pData
  Output:   VOS_VOID
  NOTE:
  Return:   VOS_VOID
  History:
      1.   欧阳飞      2009.06.15   新规作成
*******************************************************************************/
VOS_VOID RABM_SndTcCsData_Mode_I_Handling(
                      VOS_UINT8                  ucEntId,
                      RLC_RABM_CS_DATA_IND_STRU *pData
                      )
{
    VOS_INT8                       *pcData = RABM_POINTER_NULL;
    VOS_UINT32                      ulDataLen;
    VOS_UINT8                       ucSpare;
    VOS_UINT8                       ucData;
    VOS_UINT16                      usSuffix;
    VOS_UINT32                      i;

    ucSpare = (VOS_UINT8)(pData->usDataBitLen & 8);                                    /* 获得bit值                                */
    ulDataLen = 0;

    if (VOS_FALSE == g_RabmTcInf.aRbInf[ucEntId].bRlcSduSizeValid)
    {
        /*表示上行数据长度无效，直接返回，可以打印异常*/
        NAS_WARNING_LOG(WUEPS_PID_RABM, "RABM_SndTcCsData_Mode_I_Handling:WARNING:ulRlcSduSize is invalid!");
        return;
    }

    if (0 == g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize)
    {
        pcData = (VOS_INT8 *)VOS_MemAlloc(WUEPS_PID_RABM, BLOCK_MEM_PT,
                        (pData->usDataBitLen + 7) >> 3);

        if(RABM_POINTER_NULL == pcData)
        {
            NAS_ERROR_LOG(WUEPS_PID_RABM, "RABM_SndTcCsData_Mode_I_Handling:ERROR:TcTest pSendCsData Error!");
            return;
        }
        PS_NAS_MEM_CPY(pcData, pData->pucData, (pData->usDataBitLen + 7) >> 3);       /* 上下行数据长度相同                       */

        NAS_CsDomainDataReq((VOS_INT8)(g_RabmTcInf.aRbInf[ucEntId].ucRbId),
                            (VOS_UINT32)pData->usDataBitLen,
                            (VOS_UINT8 *)pcData);

        PS_RABM_MEM_FREE(pcData);
    }
    else
    {
        pcData = (VOS_INT8 *)VOS_MemAlloc(WUEPS_PID_RABM, BLOCK_MEM_PT,
                        (g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize + 7) >> 3);

        if(RABM_POINTER_NULL == pcData)
        {
            PS_LOG(WUEPS_PID_RABM, VOS_NULL,  PS_PRINT_ERROR, "RABM_SndTcCsData:ERROR:TcTest pSendCsData Error!");
            return;
        }

        if(pData->usDataBitLen >= g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize)
        {
            PS_NAS_MEM_CPY(pcData, pData->pucData,
                (g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize + 7) >> 3);           /* 复制数据                                 */
        }
        else
        {
            if(0 == ucSpare)
            {
                for(i = 1; (pData->usDataBitLen * i) <=
                        g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize; i++)
                {                                                               /* 组合上行数据                             */
                    PS_NAS_MEM_CPY(
                        (pcData + (((pData->usDataBitLen + 7) / 8) * (i - 1))),
                        pData->pucData,
                        (pData->usDataBitLen + 7) / 8);                            /* 复制数据                                 */
                    ulDataLen += pData->usDataBitLen;
                }

                if(ulDataLen < g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize)
                {                                                               /* 已经合成的数据长度小于上行数据长度       */
                    PS_NAS_MEM_CPY((pcData + ((ulDataLen + 7) / 8)),
                        pData->pucData,
                        ((g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize
                                - ulDataLen) + 7) / 8);                          /* 复制数据                                 */
                }
            }
            else
            {
                PS_NAS_MEM_CPY(pcData, pData->pucData, pData->usDataBitLen / 8);                                      /* 复制数据                                 */
                usSuffix    = (VOS_UINT8)(((pData->usDataBitLen + 7) / 8) - 1);             /* 获得数据的下标                           */
                ucData      = pData->pucData[usSuffix];
                ucData     |= pData->pucData[0] >> ucSpare;                         /* 组合数据                                 */
                ulDataLen   = pData->usDataBitLen;

                for(i = pData->usDataBitLen;i < g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize; )
                {
                    *((pcData + ((ulDataLen + 7) / 8)) -1) = (VOS_INT8)ucData;                 /* 组合上行数据                             */
                    usSuffix = (VOS_UINT16)(((ulDataLen % pData->usDataBitLen) + 7) / 8);
                    ucData = (VOS_UINT8)(pData->pucData[usSuffix] << (8-ucSpare));
                    if(usSuffix == (((pData->usDataBitLen + 7) - 8) / 8))
                    {                                                           /* 已经到达收到数据的末尾                   */
                        ulDataLen = pData->usDataBitLen;                           /* 重新初始化数据长度                       */
                        usSuffix = (VOS_UINT16)(((ulDataLen % pData->usDataBitLen)
                                    + 7) / 8);
                    }
                    else
                    {
                        usSuffix = (VOS_UINT16)((((ulDataLen % pData->usDataBitLen)
                                    + 7) / 8) +1);
                    }

                    ucData |= pData->pucData[usSuffix] >> ucSpare;
                    i += 8;
                    ulDataLen += 8;
                }
            }
        }

        ucSpare = (VOS_UINT8)(g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize % 8);

        if(0 != ucSpare)
        {
            pcData[((g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize + 7) / 8)-1]
                &= (VOS_INT8)(0xff << (8-ucSpare));
        }

        NAS_CsDomainDataReq((VOS_INT8)(g_RabmTcInf.aRbInf[ucEntId].ucRbId),
                             g_RabmTcInf.aRbInf[ucEntId].ulRlcSduSize,
                             (VOS_UINT8 *)pcData);

        PS_RABM_MEM_FREE(pcData);

    }

    return;
}


VOS_VOID RABM_SndTcCsData_Mode_II_Handling(
                      VOS_UINT8                  ucEntId,
                      RLC_RABM_CS_DATA_IND_STRU *pData
                      )
{
    VOS_UINT32                      ulDataLen;
    VOS_UINT8                       ucSpare;
    VOS_UINT8                      *pucData = RABM_POINTER_NULL;
    VOS_UINT16                      usCrcTmp;
    VOS_UINT32                      ulMaxDataLen,ulCrc,ulCrcTmp;
    VOS_UINT16                      usDataIndex;

    ucSpare = (VOS_UINT8)(pData->usDataBitLen & 8);                                    /* 获得bit值                                */
    ulDataLen = 0;

    NAS_UTRANCTRL_RABM_GetRbSuitableUlPduSize(g_RabmTcInf.aRbInf[ucEntId].ucRbId,
                                             (VOS_UINT32)pData->usDataBitLen,
                                              pData->stCrcInfo.ucCrcBitLen,
                                              &ulDataLen);

    if ( ( pData->stCrcInfo.ucCrcBitLen != 0 ) && ( pData->stCrcInfo.ucCrcBitLen != 8 )
        && ( pData->stCrcInfo.ucCrcBitLen != 12 ) && ( pData->stCrcInfo.ucCrcBitLen != 16 )
        && ( pData->stCrcInfo.ucCrcBitLen != 24 ) )
    {
        RABM_LOG_WARNING( "\n RABM_SndTcCsData:Crc Length Error! \r" );

        pucData = (VOS_UINT8 *)VOS_MemAlloc(WUEPS_PID_RABM, BLOCK_MEM_PT,
                        (ulDataLen + 7) / 8);

        if(RABM_POINTER_NULL == pucData)
        {
            RABM_LOG_WARNING( "\n [Error]TcTest pSendCsData Error! \r" );
            return;
        }

        if(pData->usDataBitLen != 0)
        {
            if ( pData->usDataBitLen <= ulDataLen )
            {
                PS_NAS_MEM_CPY( pucData, pData->pucData, ( pData->usDataBitLen + 7 ) / 8 );
            }
            else
            {
                PS_NAS_MEM_CPY( pucData, pData->pucData, ( ulDataLen + 7 ) / 8 );
            }
        }

        NAS_CsDomainDataReq((VOS_INT8)(g_RabmTcInf.aRbInf[ucEntId].ucRbId), ulDataLen, pucData);
        PS_RABM_MEM_FREE(pucData);

        return;
    }
    else if ( pData->stCrcInfo.ucCrcBitLen == 0 )
    {
        pucData = (VOS_UINT8 *)VOS_MemAlloc(WUEPS_PID_RABM, BLOCK_MEM_PT,
                        (ulDataLen + 7) / 8);

        if(RABM_POINTER_NULL == pucData)
        {
            RABM_LOG_WARNING( "\n [Error]TcTest pSendCsData Error! \r" );
            return;
        }

        if(pData->usDataBitLen != 0)
        {
            if ( pData->usDataBitLen <= ulDataLen )
            {
                PS_NAS_MEM_CPY( pucData, pData->pucData, ( pData->usDataBitLen + 7 ) / 8 );
            }
            else
            {
                PS_NAS_MEM_CPY( pucData, pData->pucData, ( ulDataLen + 7 ) / 8 );
            }
        }

        NAS_CsDomainDataReq((VOS_INT8)(g_RabmTcInf.aRbInf[ucEntId].ucRbId), ulDataLen, pucData);
        PS_RABM_MEM_FREE(pucData);
        return;
    }
    else
    {
        if ( (VOS_UINT32)( pData->usDataBitLen + pData->stCrcInfo.ucCrcBitLen ) <= ulDataLen )
        {
            ulMaxDataLen = ulDataLen;
        }
        else
        {
            ulMaxDataLen = pData->usDataBitLen + pData->stCrcInfo.ucCrcBitLen;
        }

        pucData = (VOS_UINT8 *)VOS_MemAlloc(WUEPS_PID_RABM, BLOCK_MEM_PT,
                        (ulMaxDataLen + 7) / 8);

        if(RABM_POINTER_NULL == pucData)
        {
            RABM_LOG_WARNING( "\n [Error]TcTest pSendCsData Error! \r" );
            return;
        }
        PS_NAS_MEM_SET( pucData, 0, ( ulMaxDataLen + 7 ) / 8 );

        if(pData->usDataBitLen != 0)
        {
            PS_NAS_MEM_CPY( pucData, pData->pucData, ( pData->usDataBitLen + 7 ) / 8 );
        }

        ucSpare = ( VOS_UINT8 )( pData->usDataBitLen % 8 );

        /*获取CRC*/
        ulCrc = 0;
        switch(pData->stCrcInfo.ucCrcBitLen)
        {
            case 0:
                ulCrc = 0;
                break;
            case 8:
                ulCrc = pData->stCrcInfo.aucCrc[0];
                break;
            case 12:
                ulCrc |= ((VOS_UINT32)pData->stCrcInfo.aucCrc[0] << 4) & 0x0FF0;
                ulCrc |= ((VOS_UINT32)pData->stCrcInfo.aucCrc[1] >> 4) & 0x000F;
                break;
            case 16:
                ulCrc |= ((VOS_UINT32)pData->stCrcInfo.aucCrc[0] << 8) & 0xFF00;
                ulCrc |= (VOS_UINT32)pData->stCrcInfo.aucCrc[1];
                break;
            case 24:
                ulCrc |= (VOS_UINT32)pData->stCrcInfo.aucCrc[0] << 16;
                ulCrc |= (VOS_UINT32)pData->stCrcInfo.aucCrc[1] << 8;
                ulCrc |= (VOS_UINT32)pData->stCrcInfo.aucCrc[2];
                break;
            default:
                break;
        }

        if ( 0 == ucSpare )
        {
            usDataIndex = pData->usDataBitLen / 8;
            switch ( pData->stCrcInfo.ucCrcBitLen )
            {
                case 8:
                    pucData[usDataIndex] = (VOS_UINT8)ulCrc;
                    break;
                case 12:
                    pucData[usDataIndex] = (VOS_UINT8)( ulCrc >> 4 );
                    pucData[usDataIndex + 1] = (VOS_UINT8)( ulCrc << 4 );
                    break;
                case 16:
                    pucData[usDataIndex] = (VOS_UINT8)( ulCrc >> 8 );
                    pucData[usDataIndex + 1] = (VOS_UINT8)ulCrc;
                    break;
                case 24:
                    pucData[usDataIndex] = (VOS_UINT8)( ulCrc >> 16 );
                    pucData[usDataIndex + 1] = (VOS_UINT8)( ulCrc >> 8 );
                    pucData[usDataIndex + 2] = (VOS_UINT8)ulCrc;
                    break;
                default:
                    break;
            }
        }
        else
        {
            usDataIndex = pData->usDataBitLen / 8;
            switch ( pData->stCrcInfo.ucCrcBitLen )
            {
                case 8:
                    ulCrc &= 0x000000ff;
                    usCrcTmp = (VOS_UINT16)( ( ulCrc << ( 8 - ucSpare ) ) );
                    pucData[usDataIndex] &= 0Xff << ( 8 - ucSpare );
                    pucData[usDataIndex] |= (VOS_UINT8)( usCrcTmp >> 8 );
                    pucData[usDataIndex + 1] = (VOS_UINT8)( usCrcTmp );
                    break;
                case 12:
                    ulCrc &= 0x00000fff;
                    if ( ucSpare <= 4 )
                    {
                        usCrcTmp = (VOS_UINT16)( ulCrc << ( 4 - ucSpare ) );
                        pucData[usDataIndex] &= 0xff << ( 8 - ucSpare );
                        pucData[usDataIndex] |= (VOS_UINT8)( usCrcTmp >> 8 );
                        pucData[usDataIndex + 1] = (VOS_UINT8)( usCrcTmp );
                    }
                    else
                    {
                        ulCrcTmp = ulCrc << ( 12 - ucSpare );
                        pucData[usDataIndex] &= 0xff << ( 8 - ucSpare );
                        pucData[usDataIndex] |= (VOS_UINT8)( ulCrcTmp >> 16 );
                        pucData[usDataIndex + 1] = (VOS_UINT8)( ulCrcTmp >> 8 );
                        pucData[usDataIndex + 2] = (VOS_UINT8)( ulCrcTmp );
                    }
                    break;
                case 16:
                    ulCrc &= 0x0000ffff;
                    ulCrcTmp = ulCrc << ( 8 - ucSpare );
                    pucData[usDataIndex] &= 0xff << ( 8 - ucSpare );
                    pucData[usDataIndex] |= (VOS_UINT8)( ulCrcTmp >> 16 );
                    pucData[usDataIndex + 1] = (VOS_UINT8)( ulCrcTmp >> 8 );
                    pucData[usDataIndex + 2] = (VOS_UINT8)( ulCrcTmp );
                    break;
                case 24:
                    ulCrc &= 0x00ffffff;
                    ulCrcTmp = ulCrc << ( 8 - ucSpare );
                    pucData[usDataIndex] &= 0xff << ( 8 - ucSpare );
                    pucData[usDataIndex] |= (VOS_UINT8)( ulCrcTmp >> 24 );
                    pucData[usDataIndex + 1] = (VOS_UINT8)( ulCrcTmp >> 16 );
                    pucData[usDataIndex + 2] = (VOS_UINT8)( ulCrcTmp >> 8 );
                    pucData[usDataIndex + 3] = (VOS_UINT8)( ulCrcTmp );
                    break;
                default:
                    break;
            }
        }
        NAS_CsDomainDataReq( (VOS_INT8)(g_RabmTcInf.aRbInf[ucEntId].ucRbId), ulDataLen, pucData);
        PS_RABM_MEM_FREE(pucData);
    }

    return;
}


VOS_VOID RABM_SndTcCsData(
                      VOS_UINT8 ucEntId,
                      RLC_RABM_CS_DATA_IND_STRU *pData
                      )
{
    if ( g_RabmTcInf.ucTestLoopMode == TC_MODE_1 )
    {
        RABM_SndTcCsData_Mode_I_Handling(ucEntId, pData);
    }
    else
    {
        RABM_SndTcCsData_Mode_II_Handling(ucEntId, pData);
    }

    return;
}


VOS_VOID RABM_SndTcRabInd(
                      VOS_UINT8  ucEndId,
                      VOS_UINT8  ucSetUpFlg
                      )
{
    TCRABM_RAB_IND_STRU         *pSndMsg;

    pSndMsg = (TCRABM_RAB_IND_STRU *)
                PS_ALLOC_MSG(WUEPS_PID_RABM, sizeof(TCRABM_RAB_IND_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pSndMsg)
    {
        PS_LOG(WUEPS_PID_RABM, VOS_NULL, PS_PRINT_ERROR, "RABM_SndTcRabInd:ERROR:Alloc msg fail!");
        return;
    }

    pSndMsg->MsgHeader.ulSenderCpuId = VOS_LOCAL_CPUID;
    pSndMsg->MsgHeader.ulSenderPid = WUEPS_PID_RABM;
    pSndMsg->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pSndMsg->MsgHeader.ulReceiverPid = WUEPS_PID_TC;
    pSndMsg->MsgHeader.ulMsgName = TCRABM_RAB_IND;

    pSndMsg->ulRabChangeType = ucSetUpFlg;                                      /* 操作类型                                 */
    pSndMsg->RabInfo.ulCnDomainId = g_RabmTcInf.aRbInf[ucEndId].ucDomain;       /* CN DOMAIN                                */
    pSndMsg->RabInfo.ulRabId = g_RabmTcInf.aRbInf[ucEndId].ucRabId;             /* RAB ID                                   */
    pSndMsg->RabInfo.ulRbId = g_RabmTcInf.aRbInf[ucEndId].ucRbId;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_RABM, pSndMsg))
    {
        NAS_WARNING_LOG(WUEPS_PID_RABM, "RABM_SndTcRabInd():WARNING:SEND MSG FIAL");
    }
    return;
}

/*******************************************************************************
  Module:   RABM_SndTafStatusInd
  Function: 通知APP启动或停止数据传输
  Input:    VOS_UINT8                   ucNsapiOrSi     RAB ID所对应的NSAPI或SI
            VOS_UINT8                   ucStatusFlg     停止或开始发送数据标志
            VOS_UINT8                   ucCause         开始发送数据的原因
  Output:   VOS_VOID
  NOTE:
  Return:   VOS_VOID
  History:
      1.   张志勇   2005.01.07   新规作成
*******************************************************************************/
VOS_VOID RABM_SndTafStatusInd(
                          VOS_UINT8 ucNsapiOrSi,                                    /* RAB ID所对应的NSAPI或SI                  */
                          VOS_UINT8 ucStatusFlg,                                    /* 停止或开始发送数据标志                   */
                          VOS_UINT8 ucCause                                         /* 开始发送数据的原因                       */
                          )
{
    ST_APP_RABM_STATUS   TafStatusInd;                                          /* 定义局部结构体变量                       */

    TafStatusInd.ucDomain     = D_RABM_PS_DOMAIN;                               /* 填写域值                                 */
    TafStatusInd.ucRabId      = ucNsapiOrSi;                                    /* 保存RAB ID所对应的NSAPI或SI              */
    TafStatusInd.ucStatus     = ucStatusFlg;                                    /* 保存停止或开始发送数据标志               */
    TafStatusInd.ucStartCause = ucCause;                                        /* 保存开始发送数据的原因                   */

    Api_AppRabmStatusInd( &TafStatusInd );                                      /* 通知APP启动或停止数据传输                */
}

/*******************************************************************************
  Module:   RABM_SndTafInActNsapiInd
  Function: 通知APP指定的NSAPI没有激活
  Input:    VOS_UINT8                   ucNsapi     RAB ID所对应的NSAPI
  Output:   VOS_VOID
  NOTE:
  Return:   VOS_VOID
  History:
      1.   张志勇   2005.01.07   新规作成
*******************************************************************************/
VOS_VOID RABM_SndTafInActNsapiInd( VOS_UINT8 ucNsapi )
{
    ST_APP_RABM_ERROR   TafInActNsapiInd;                                       /* 定义局部结构体变量                       */

    TafInActNsapiInd.ucDomain  = D_RABM_PS_DOMAIN;                               /* 填写域值                                 */
    TafInActNsapiInd.ucRabId   = ucNsapi;                                         /* 保存RAB ID所对应的NSAPI                  */
    TafInActNsapiInd.ucErrType = TAF_DATA_CAUSE_LOWLAYER_INDICATION;

    Api_AppRabmErrInd( &TafInActNsapiInd );                                     /* 通知APP指定的NSAPI没有激活               */
}

/*******************************************************************************
  Module:   RABM_SndTafInActSiInd
  Function: 通知APP指定的SI没有激活
  Input:    VOS_UINT8                   ucSi        RAB ID所对应的SI
  Output:   VOS_VOID
  NOTE:
  Return:   VOS_VOID
  History:
      1.   张志勇   2005.01.06   新规作成
*******************************************************************************/
VOS_VOID RABM_SndTafInActSiInd( VOS_UINT8 ucSi )
{
    ST_APP_RABM_ERROR   TafInActSiInd;                                          /* 定义局部结构体变量                       */

    TafInActSiInd.ucDomain  = D_RABM_CS_DOMAIN;                                  /* 填写域值                                 */
    TafInActSiInd.ucRabId   = ucSi;                                               /* 保存RAB ID所对应的SI                     */
    TafInActSiInd.ucErrType = TAF_DATA_CAUSE_LOWLAYER_INDICATION;

    Api_AppRabmErrInd( &TafInActSiInd );                                        /* 通知APP指定的SI没有激活                  */
}

VOS_VOID RABM_SndGmmRabRelInd( VOS_VOID )
{
    GMMRABM_RAB_REL_IND_STRU    *pSndMsg;

    pSndMsg = (GMMRABM_RAB_REL_IND_STRU *)
                PS_ALLOC_MSG(WUEPS_PID_RABM, sizeof(GMMRABM_RAB_REL_IND_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pSndMsg)
    {
        PS_LOG(WUEPS_PID_RABM, VOS_NULL, PS_PRINT_ERROR, "RABM_SndGmmRabRelInd:ERROR:Alloc msg fail!");
        return;
    }

    pSndMsg->MsgHeader.ulSenderCpuId = VOS_LOCAL_CPUID;
    pSndMsg->MsgHeader.ulSenderPid = WUEPS_PID_RABM;
    pSndMsg->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pSndMsg->MsgHeader.ulReceiverPid = WUEPS_PID_GMM;
    pSndMsg->MsgHeader.ulMsgName = ID_RABM_GMM_RAB_REL_IND;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_RABM, pSndMsg))
    {
        NAS_WARNING_LOG(WUEPS_PID_RABM, "RABM_SndGmmRabRelInd():WARNING:SEND MSG FIAL");
    }
    return;
}


VOS_VOID NAS_RABM_SndGmmRbSetupInd(VOS_VOID)
{
    GMMRABM_RAB_SETUP_IND_STRU         *pstMsg = VOS_NULL_PTR;

    pstMsg = (GMMRABM_RAB_SETUP_IND_STRU *)
                PS_ALLOC_MSG(WUEPS_PID_RABM, sizeof(GMMRABM_RAB_SETUP_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        PS_LOG(WUEPS_PID_RABM, VOS_NULL, PS_PRINT_ERROR, "NAS_RABM_SndGmmRbSetupInd:ERROR:Alloc msg fail!");
        return;
    }

    pstMsg->MsgHeader.ulSenderCpuId = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid = WUEPS_PID_RABM;
    pstMsg->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid   = WUEPS_PID_GMM;
    pstMsg->MsgHeader.ulMsgName       = ID_RABM_GMM_RAB_SETUP_IND;
    pstMsg->MsgHeader.ulLength        = sizeof(GMMRABM_RAB_SETUP_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_RABM, pstMsg))
    {
        NAS_WARNING_LOG(WUEPS_PID_RABM, "NAS_RABM_SndGmmRbSetupInd():WARNING:SEND MSG FIAL");
    }

    return;
}


VOS_UINT32 NAS_RABM_SendApsSysChgInd(
    VOS_UINT32                          ulMsgId
)
{
    RABM_APS_SYS_CHANGE_MSG            *pstSysChgInd;

    pstSysChgInd = (RABM_APS_SYS_CHANGE_MSG*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                            WUEPS_PID_RABM,
                            sizeof(RABM_APS_SYS_CHANGE_MSG));
    if (VOS_NULL_PTR == pstSysChgInd)
    {
        NAS_ERROR_LOG1(WUEPS_PID_RABM,
            "NAS_RABM_SendApsSysChgInd: Memory alloc failed, MsgID:", ulMsgId);
        return VOS_ERR;
    }

    /* 填写消息头 */
    pstSysChgInd->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstSysChgInd->ulReceiverPid   = WUEPS_PID_TAF;
    pstSysChgInd->ulMsgType       = ulMsgId;

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_RABM, pstSysChgInd))
    {
        NAS_ERROR_LOG1(WUEPS_PID_RABM,
            "NAS_RABM_SendApsSysChgInd: Send message failed, MsgID:", ulMsgId);
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_VOID NAS_RABM_SndWasFastDormStartReq(
    RABM_RELRRC_OPERATE_ENUM_UINT32     enOptFlag
)
{
    VOS_UINT32                              ulRet;                              /* 定义调用VOS发送函数的返回值 */
    RRRABM_FASTDORM_START_REQ_STRU         *pstMsg = VOS_NULL_PTR;              /* 定义原语类型指针 */
    RRC_FASTDORM_START_TYPE_ENUM_UINT32     enWrrFastDormStartType;


    enWrrFastDormStartType = RRC_FASTDORM_START_TYPE_BUTT;


    /* 申请内存  */
    pstMsg = (RRRABM_FASTDORM_START_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_RABM,
                                               sizeof(RRRABM_FASTDORM_START_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstMsg )
    {
        /* 内存申请失败 */
        NAS_ERROR_LOG(WUEPS_PID_RABM, "NAS_RABM_SndWasFastDormStartReq:ERROR: Memory Alloc Error for pstMsg");
        return ;
    }

    /* 填写相关参数 */
    pstMsg->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid     = WUEPS_PID_WRR;
    pstMsg->MsgHeader.ulLength          = sizeof(RRRABM_FASTDORM_START_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName         = RRRABM_FASTDORM_START_REQ;

    /* 将AT的操作类型转换成Wrr对应的操作类型 */
    if (RABM_FASTDORM_ENUM == enOptFlag)
    {
        NAS_RABM_ConvertAtFasTDormTypeToWrrFasTDormType(NAS_RABM_GetFastDormOperationType(),
                                                        &enWrrFastDormStartType);
    }
    else
    {
        /* release rrc 命令同时开启FD和ASCR */
        NAS_RABM_ConvertAtFasTDormTypeToWrrFasTDormType(AT_RABM_FASTDORM_START_FD_ASCR,
                                                        &enWrrFastDormStartType);
    }

    pstMsg->enStartType                 = enWrrFastDormStartType;

    /* 调用VOS发送原语 */
     ulRet = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_RABM, (struct MsgCB **)&pstMsg);
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM, "NAS_RABM_SndWasFastDormStartReq:ERROR:PS_SEND_MSG ");
        return ;
    }

}



VOS_VOID NAS_RABM_SndWasFastDormStopReq(VOS_VOID)
{
    VOS_UINT32                          ulRet;                                  /* 定义调用VOS发送函数的返回值 */
    RRRABM_FASTDORM_STOP_REQ_STRU     *pstMsg = VOS_NULL_PTR;                  /* 定义原语类型指针 */

    /* 申请内存  */
    pstMsg = (RRRABM_FASTDORM_STOP_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_RABM,
                                               sizeof(RRRABM_FASTDORM_STOP_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstMsg )
    {
        /* 内存申请失败 */
        NAS_ERROR_LOG(WUEPS_PID_RABM, "NAS_RABM_SndWasFastDormStopReq:ERROR: Memory Alloc Error for pstMsg");
        return ;
    }

    /* 填写相关参数 */
    pstMsg->MsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid     = WUEPS_PID_WRR;
    pstMsg->MsgHeader.ulLength          = sizeof(RRRABM_FASTDORM_STOP_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->MsgHeader.ulMsgName         = RRRABM_FASTDORM_STOP_REQ;

    /* 调用VOS发送原语 */
    ulRet = NAS_UTRANCTRL_SndAsMsg(WUEPS_PID_RABM, (struct MsgCB **)&pstMsg);

    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM, "NAS_RABM_SndWasFastDormStopReq:ERROR:PS_SEND_MSG ");
        return ;
    }

}



VOS_VOID NAS_RABM_SndGmmMmlProcStatusQryReq(
    RABM_RELRRC_OPERATE_ENUM_UINT32     enOptFlag
)
{
    VOS_UINT32                                  ulRet;                                  /* 定义调用VOS发送函数的返回值 */
    RABM_GMM_MML_PROC_STATUS_QRY_REQ_STRU      *pstMsg = VOS_NULL_PTR;                  /* 定义原语类型指针 */

    /* 申请内存  */
    pstMsg = (RABM_GMM_MML_PROC_STATUS_QRY_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_RABM,
                                               sizeof(RABM_GMM_MML_PROC_STATUS_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstMsg )
    {
        /* 内存申请失败 */
        NAS_ERROR_LOG(WUEPS_PID_RABM, "NAS_RABM_SndGmmMmlProcStatusQryReq:ERROR: Memory Alloc Error for pstMsg");
        return ;
    }

    /* 填写相关参数 */
    pstMsg->stMsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverPid     = WUEPS_PID_GMM;
    pstMsg->stMsgHeader.ulLength          = sizeof(RABM_GMM_MML_PROC_STATUS_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->stMsgHeader.ulMsgName         = ID_RABM_GMM_MML_PROC_STATUS_QRY_REQ;

    pstMsg->enOptFlag                     = enOptFlag;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG(WUEPS_PID_RABM, pstMsg);
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM, "NAS_RABM_SndGmmMmlProcStatusQryReq:ERROR:PS_SEND_MSG ");
        return ;
    }

}



VOS_VOID NAS_RABM_SndAtSetFastDormParaCnf(
    MN_CLIENT_ID_T                      usClientId,                             /* AT发消息给RABM所带的ClientId */
    MN_OPERATION_ID_T                   ucOpId,                                 /* AT发消息给RABM所带的OpId */
    AT_RABM_PARA_SET_RSLT_ENUM_UINT32   enRslt
)
{
    /* 向AT发送设置结果 */
    VOS_UINT32                          ulRet;                                  /* 定义调用VOS发送函数的返回值 */
    RABM_AT_SET_FASTDORM_PARA_CNF_STRU *pstMsg = VOS_NULL_PTR;                  /* 定义原语类型指针 */

    /* 申请内存  */
    pstMsg = (RABM_AT_SET_FASTDORM_PARA_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_RABM,
                                               sizeof(RABM_AT_SET_FASTDORM_PARA_CNF_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstMsg )
    {
        /* 内存申请失败 */
        NAS_ERROR_LOG(WUEPS_PID_RABM, "NAS_RABM_SndAtSetFastDormParaCnf:ERROR: Memory Alloc Error for pstMsg");
        return ;
    }

    /* 填写相关参数 */
    pstMsg->stMsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverPid     = WUEPS_PID_AT;
    pstMsg->stMsgHeader.ulLength          = sizeof(RABM_AT_SET_FASTDORM_PARA_CNF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->stMsgHeader.ulMsgName         = ID_RABM_AT_SET_FASTDORM_PARA_CNF;
    pstMsg->enRslt                        = enRslt;
    pstMsg->usClientId                    = usClientId;
    pstMsg->ucOpId                        = ucOpId;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG(WUEPS_PID_RABM, pstMsg);
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM, "NAS_RABM_SndAtSetFastDormParaCnf:ERROR:PS_SEND_MSG ");
        return ;
    }

    return ;
}
VOS_VOID NAS_RABM_SndAtQryFastDormParaCnf(
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId,
    VOS_UINT8                           ucRslt
)
{
    /* 向AT发送查询结果 */
    VOS_UINT32                          ulRet;                                  /* 定义调用VOS发送函数的返回值 */
    RABM_AT_QRY_FASTDORM_PARA_CNF_STRU *pstMsg = VOS_NULL_PTR;                  /* 定义原语类型指针 */

    /* 申请内存  */
    pstMsg = (RABM_AT_QRY_FASTDORM_PARA_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_RABM,
                                               sizeof(RABM_AT_QRY_FASTDORM_PARA_CNF_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstMsg )
    {
        /* 内存申请失败 */
        NAS_ERROR_LOG(WUEPS_PID_RABM, "NAS_RABM_SndAtQryFastDormParaCnf:ERROR: Memory Alloc Error for pstMsg");
        return ;
    }

    /* 填写相关参数 */
    pstMsg->stMsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverPid     = WUEPS_PID_AT;
    pstMsg->stMsgHeader.ulLength          = sizeof(RABM_AT_QRY_FASTDORM_PARA_CNF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->stMsgHeader.ulMsgName         = ID_RABM_AT_QRY_FASTDORM_PARA_CNF;
    pstMsg->usClientId                    = usClientId;
    pstMsg->ucOpId                        = ucOpId;
    pstMsg->ucRslt                        = ucRslt;
    pstMsg->stFastDormPara.enFastDormOperationType = NAS_RABM_GetFastDormOperationType();
    pstMsg->stFastDormPara.ulTimeLen = NAS_RABM_GetFastDormUserDefNoFluxCntValue();

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG(WUEPS_PID_RABM, pstMsg);
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM, "NAS_RABM_SndAtQryFastDormParaCnf:ERROR:PS_SEND_MSG ");
        return ;
    }

    return ;
}
VOS_VOID  NAS_RABM_SndOmFastdormStatus(VOS_VOID)
{
    NAS_RABM_LOG_FASTDORM_INFO_STRU    *pstMsg;

    pstMsg = (NAS_RABM_LOG_FASTDORM_INFO_STRU*)PS_MEM_ALLOC(WUEPS_PID_RABM,
                             sizeof(NAS_RABM_LOG_FASTDORM_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_RABM_SndOmFastdormStatus:ERROR:Alloc Mem Fail.");
        return;
    }

    pstMsg->stMsgHeader.ulReceiverCpuId           = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid               = WUEPS_PID_RABM;
    pstMsg->stMsgHeader.ulReceiverPid             = WUEPS_PID_RABM;

    pstMsg->stMsgHeader.ulLength                  = sizeof(NAS_RABM_LOG_FASTDORM_INFO_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->stMsgHeader.ulMsgName                 = NAS_RABM_OM_MSG_FASTDORM_STATUS;
    pstMsg->stFastDormCtx.enCurrFastDormStatus    = NAS_RABM_GetCurrFastDormStatus();
    pstMsg->stFastDormCtx.enFastDormOperationType = NAS_RABM_GetFastDormOperationType();
    pstMsg->stFastDormCtx.ulUserDefNoFluxCnt      = NAS_RABM_GetFastDormUserDefNoFluxCntValue();
    pstMsg->stFastDormCtx.ulCurrNoFluxCnt         = NAS_RABM_GetFastDormCurrNoFluxCntValue();

    pstMsg->stFastDormCtx.ulDlDataCnt             = NAS_RABM_GetFastDormDlDataCnt();
    pstMsg->stFastDormCtx.ulUlDataCnt             = NAS_RABM_GetFastDormUlDataCnt();
    pstMsg->stFastDormCtx.ulRelRrcExecFlg         = NAS_RABM_GET_FD_REL_RRC_EXEC_FLG();

    OM_TraceMsgHook(pstMsg);

    PS_MEM_FREE(WUEPS_PID_RABM, pstMsg);

}




VOS_VOID  NAS_RABM_SndCdsFreeBuffDataInd(VOS_UINT8 ucRabId)
{
    CDS_RABM_FREE_BUFF_DATA_IND_STRU    *pstMsg;

    pstMsg = (CDS_RABM_FREE_BUFF_DATA_IND_STRU*)PS_ALLOC_MSG(WUEPS_PID_RABM,
                             sizeof(CDS_RABM_FREE_BUFF_DATA_IND_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM, "NAS_RABM_SndCdsFreeBuffDataInd:ERROR:Alloc Mem Fail.");
        return;
    }

    /* 消息赋值 */
    pstMsg->ulReceiverCpuId     = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid       = UEPS_PID_CDS;
    pstMsg->enMsgID             = ID_RABM_CDS_FREE_BUFF_DATA_IND;
    pstMsg->ucRabId             = ucRabId;

    PS_MEM_SET(pstMsg->aucReserved, 0, 3);

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_RABM, pstMsg))
    {
        NAS_WARNING_LOG(WUEPS_PID_RABM, "NAS_RABM_SndCdsFreeBuffDataInd():WARNING:SEND MSG FIAL");
    }
    return;
}


VOS_VOID  NAS_RABM_SndCdsSendBuffDataInd(
    VOS_UINT8                                               ucRabId,
    CDS_RABM_SEND_BUFF_DATA_ALLOWED_TYPE_ENUM_UINT32        enSndBuffDataAllowedType
)
{
    CDS_RABM_SEND_BUFF_DATA_IND_STRU    *pstMsg;

    pstMsg = (CDS_RABM_SEND_BUFF_DATA_IND_STRU*)PS_ALLOC_MSG(WUEPS_PID_RABM,
                             sizeof(CDS_RABM_SEND_BUFF_DATA_IND_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM, "NAS_RABM_SndCdsSendBuffDataInd:ERROR:Alloc Mem Fail.");
        return;
    }

    /* 消息赋值 */
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = UEPS_PID_CDS;
    pstMsg->enMsgId                     = ID_RABM_CDS_SEND_BUFF_DATA_IND;
    pstMsg->enSndBuffDataAllowedType    = enSndBuffDataAllowedType;
    pstMsg->ucRabId                     = ucRabId;

    PS_MEM_SET(pstMsg->aucReserved, 0, 3);

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_RABM, pstMsg))
    {
        NAS_WARNING_LOG(WUEPS_PID_RABM, "NAS_RABM_SndCdsSendBuffDataInd():WARNING:SEND MSG FIAL");
    }

    return;
}


VOS_VOID  NAS_RABM_SndCdsQosFcRabCreateInd(
    VOS_UINT8                           ucRabId,
    QCI_TYPE_ENUM_UINT8                 enQci
)
{
    QOS_FC_RAB_CREATE_IND_STRU         *pstMsg;

    pstMsg = (QOS_FC_RAB_CREATE_IND_STRU*)PS_ALLOC_MSG(WUEPS_PID_RABM,
                             sizeof(QOS_FC_RAB_CREATE_IND_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM, "NAS_RABM_SndCdsQosFcRabCreateInd:ERROR:Alloc Mem Fail.");
        return;
    }

    /* 消息赋值 */
    pstMsg->ulReceiverCpuId     = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid       = UEPS_PID_CDS;
    pstMsg->enMsgId             = ID_QOS_FC_RABM_RAB_CREATE_IND;

    pstMsg->ucRabId             = ucRabId;

    /* 目前不支持Second Pdp，ucLinkRabId与主PDP相同 */
    pstMsg->ucLinkRabId         = ucRabId;
    pstMsg->enQci               = enQci;

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_RABM, pstMsg))
    {
        NAS_WARNING_LOG(WUEPS_PID_RABM, "NAS_RABM_SndCdsQosFcRabCreateInd():WARNING:SEND MSG FIAL");
    }
    return;
}
VOS_VOID  NAS_RABM_SndCdsQosFcRabReleaseInd(
    VOS_UINT8                           ucRabId
)
{
    QOS_FC_RAB_RELEASE_IND_STRU        *pstMsg;

    pstMsg = (QOS_FC_RAB_RELEASE_IND_STRU*)PS_ALLOC_MSG(WUEPS_PID_RABM,
                             sizeof(QOS_FC_RAB_RELEASE_IND_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM, "NAS_RABM_SndCdsQosFcRabReleaseInd:ERROR:Alloc Mem Fail.");
        return;
    }

    /* 消息赋值 */
    pstMsg->ulReceiverCpuId     = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid       = UEPS_PID_CDS;
    pstMsg->enMsgId             = ID_QOS_FC_RABM_RAB_RELEASE_IND;
    pstMsg->ucRabId             = ucRabId;

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_RABM, pstMsg))
    {
        NAS_WARNING_LOG(WUEPS_PID_RABM, "NAS_RABM_SndCdsQosFcRabReleaseInd():WARNING:SEND MSG FIAL");
    }
    return;
}




VOS_VOID NAS_RABM_SndAtReleaseRrcCnf(
    VOS_UINT16                          usClientId,                             /* AT发消息给RABM所带的ClientId */
    VOS_UINT8                           ucOpId,                                 /* AT发消息给RABM所带的OpId */
    VOS_UINT32                          ulRslt
)
{
    /* 向AT发送设置结果 */
    VOS_UINT32                          ulRet;                                  /* 定义调用VOS发送函数的返回值 */
    RABM_AT_RELEASE_RRC_CNF_STRU       *pstMsg = VOS_NULL_PTR;                  /* 定义原语类型指针 */

    /* 申请内存  */
    pstMsg = (RABM_AT_RELEASE_RRC_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_RABM,
                              sizeof(RABM_AT_RELEASE_RRC_CNF_STRU) - VOS_MSG_HEAD_LENGTH);
    if ( VOS_NULL_PTR == pstMsg )
    {
        /* 内存申请失败 */
        NAS_ERROR_LOG(WUEPS_PID_RABM, "NAS_RABM_SndAtReleaseRrcCnf:ERROR: Memory Alloc Error for pstMsg");
        return ;
    }

    PS_MEM_SET((VOS_CHAR*)pstMsg + VOS_MSG_HEAD_LENGTH, 0x00, (sizeof(RABM_AT_RELEASE_RRC_CNF_STRU) - VOS_MSG_HEAD_LENGTH));

    /* 填写相关参数 */
    pstMsg->stMsgHeader.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverPid     = WUEPS_PID_AT;
    pstMsg->stMsgHeader.ulLength          = sizeof(RABM_AT_RELEASE_RRC_CNF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->stMsgHeader.ulMsgName         = ID_RABM_AT_SET_RELEASE_RRC_CNF;
    pstMsg->ulRslt                        = ulRslt;
    pstMsg->usClientId                    = usClientId;
    pstMsg->ucOpId                        = ucOpId;

    /* 调用VOS发送原语 */
    ulRet = PS_SEND_MSG(WUEPS_PID_RABM, pstMsg);
    if ( VOS_OK != ulRet )
    {
        NAS_ERROR_LOG(WUEPS_PID_RABM, "NAS_RABM_SndAtReleaseRrcCnf:ERROR:PS_SEND_MSG ");
        return ;
    }

    return ;
}

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

