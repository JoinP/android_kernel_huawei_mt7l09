


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "NasEmmAttDetInclude.h"
#include    "NasLmmPubMPrint.h"
#include    "NasMmlCtx.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMATTACH_C
/*lint +e767*/

/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/
NAS_EMM_ATTACH_CTRL_STRU                g_stEmmAttCtrl;

/*****************************************************************************
  3 Function
*****************************************************************************/


/*lint -e960*/
/*lint -e961*/
VOS_VOID  NAS_EMM_AttachInit()
{
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_AttachInit                START INIT...");

    /* ��ʼ��ATTACHȫ�ֱ��� */
    NAS_LMM_MEM_SET(&g_stEmmAttCtrl, 0, sizeof(g_stEmmAttCtrl));
    /*NAS_EMM_GLO_AD_GetAttRst()          = EMM_ATTACH_RST_PS;*/
    /*NAS_EMM_GLO_AD_GetAttType()         = MMC_LMM_ATT_TYPE_EPS_ONLY;*/

    return;
}
VOS_VOID  NAS_EMM_FreeAttDyn( VOS_VOID )
{
    /*��ʱ�޲���*/

    return;
}
VOS_VOID     NAS_EMM_Attach_ClearResourse(VOS_VOID)
{

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_Attach_ClearResourse is entered");

    /*��̬�ڴ��ͷ�*/
    NAS_EMM_FreeAttDyn();

    /* ֹͣATTACH��ʱ�� */
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3402);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_T3410);
    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_T3411);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_ESM_BEARER_CNF);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_ESM_PDN_RSP);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_RRC_DATA_CNF);

    /*����ֵ*/
    NAS_LMM_MEM_SET(NAS_EMM_GLO_AD_GetAttCtrlAddr(), 0, sizeof(NAS_EMM_ATTACH_CTRL_STRU));
    /*NAS_EMM_GLO_AD_GetAttRst()          = EMM_ATTACH_RST_PS;*/

    /*NAS_EMM_GLO_AD_GetAttType()         = MMC_LMM_ATT_TYPE_EPS_ONLY;*/
    NAS_EMM_SetAttType();

    return;
}

/*****************************************************************************
 Function Name   :  NAS_EMM_Attach_SuspendInitClearResourse
 Description     :  �ڹ���ʱ����ATTACH��Դ
 Input           :

 Return          :

 History         :
    1.FTY         2012-02-21  Draft Enact

*****************************************************************************/
VOS_VOID     NAS_EMM_Attach_SuspendInitClearResourse(VOS_VOID)
{

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_Attach_SuspendInitClearResourse is entered");

    /*��̬�ڴ��ͷ�*/
    NAS_EMM_FreeAttDyn();

    /* ֹͣATTACH��ʱ�� */
    NAS_LMM_StopStateTimer(TI_NAS_EMM_T3410);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_ESM_BEARER_CNF);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_ESM_PDN_RSP);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_RRC_DATA_CNF);

    /*����ֵ*/
    NAS_LMM_MEM_SET(NAS_EMM_GLO_AD_GetAttCtrlAddr(), 0, sizeof(NAS_EMM_ATTACH_CTRL_STRU));
    /*NAS_EMM_GLO_AD_GetAttRst()          = EMM_ATTACH_RST_PS;*/

    /*NAS_EMM_GLO_AD_GetAttType()         = MMC_LMM_ATT_TYPE_EPS_ONLY;*/
    NAS_EMM_SetAttType();

    return;
}
VOS_VOID  NAS_EMM_SendRrcClearBuffNotify(VOS_VOID )
{
    LRRC_LMM_CLEAR_BUFF_NOTIFY_STRU           *pstCleBuffNotifyIndMsg;

    /*��ӡ����ú���*/
    NAS_EMM_ATTACH_LOG_NORM(            "NAS_EMM_SendRrcClearBuffNotify is entered");

    /* ����DOPRA��Ϣ */
    pstCleBuffNotifyIndMsg = (VOS_VOID *) NAS_LMM_ALLOC_MSG(sizeof(LRRC_LMM_CLEAR_BUFF_NOTIFY_STRU));
    if (NAS_LMM_NULL_PTR == pstCleBuffNotifyIndMsg)
    {
        /* ��ӡ�쳣��ERROR_LEVEL */
        NAS_EMM_PUBU_LOG_ERR("NAS_EMM_SendRrcClearBuffNotify: MSG ALLOC ERROR!!!");
        return ;
    }

    /* ��� */
    NAS_LMM_MEM_SET(pstCleBuffNotifyIndMsg,0,sizeof(LRRC_LMM_CLEAR_BUFF_NOTIFY_STRU));

    /* ���VOS��Ϣͷ */
    NAS_EMM_SET_RRC_MSG_HEADER((pstCleBuffNotifyIndMsg),
                             NAS_EMM_GET_MSG_LENGTH_NO_HEADER(LRRC_LMM_CLEAR_BUFF_NOTIFY_STRU));

    /* �����ϢID */
    pstCleBuffNotifyIndMsg->enMsgId        = ID_LRRC_LMM_CLEAR_BUFF_NOTIFY;

    /* ����DOPRA��Ϣ */
    NAS_LMM_SEND_MSG(pstCleBuffNotifyIndMsg);

    return;
}
VOS_VOID  NAS_EMM_AdStateConvert(NAS_EMM_MAIN_STATE_ENUM_UINT16 ucMs,
                                   NAS_EMM_SUB_STATE_ENUM_UINT16 ucSs,
                                   NAS_EMM_STATE_TI_ENUM_UINT16 ucStaTId)
{
    NAS_EMM_FSM_STATE_STRU              stDestState;

    stDestState.enFsmId                 = NAS_LMM_PARALLEL_FSM_EMM;
    stDestState.enMainState             = ucMs;
    stDestState.enSubState              = ucSs;
    stDestState.enStaTId                = ucStaTId;
    NAS_LMM_StaTransProc(stDestState);

}


VOS_VOID    NAS_EMM_EsmSendRelInd()
{
    EMM_ESM_REL_IND_STRU                 *pEmmEsmRelIndMsg;


    /*������Ϣ�ڴ�*/
    pEmmEsmRelIndMsg = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(EMM_ESM_REL_IND_STRU));

    /*�ж�����������ʧ�ܴ�ӡ�����˳�*/
    if (NAS_EMM_NULL_PTR == pEmmEsmRelIndMsg)
    {
        /*��ӡ����*/
        NAS_EMM_ATTACH_LOG_ERR(" NAS_EMM_EsmSendRelInd: MSG ALLOC ERR!");
        return;

    }

    /*����ID_EMM_ESM_REL_IND��Ϣ*/
    /*�����Ϣͷ*/
    NAS_EMM_COMP_AD_ESM_MSG_HEADER(      pEmmEsmRelIndMsg,
                                        (sizeof(EMM_ESM_REL_IND_STRU)-
                                        NAS_EMM_LEN_VOS_MSG_HEADER));

    /*�����ϢID*/
    pEmmEsmRelIndMsg->ulMsgId            = ID_EMM_ESM_REL_IND;

    /*��ESM����ID_EMM_ESM_REL_IND��Ϣ*/
    NAS_LMM_SEND_MSG(                   pEmmEsmRelIndMsg);

    return;


}
VOS_UINT32  NAS_EMM_IntraAttReqChk(const NAS_LMM_INTRA_ATTACH_REQ_STRU* pMsgMsg)
{
    /*��Ϣ���ݼ��*/
    if (ID_NAS_LMM_INTRA_ATTACH_REQ == pMsgMsg->ulMsgId)
    {
        return  NAS_EMM_PARA_VALID;

    }
    else
    {
        return  NAS_EMM_PARA_INVALID;

    }
}

VOS_VOID    NAS_EMM_AtmpCntLess5Proc()
{
    /*��ӡ����ú���*/
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_AtmpCntLess5Proc is entered");

    /*������ʱ��T3411*/
    NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3411);

    /*�޸�״̬��������״̬DEREG��״̬DEREG_ATTEMPTING_TO_ATTACH*/
    NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                        EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                        TI_NAS_EMM_STATE_NO_TIMER);

}
VOS_VOID    NAS_EMM_AtmpCntEqual5Proc()
{

    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_AtmpCntEqual5Proc is entered");

    /* ɾ��GUTI����Զ�����EPS_LOC,������Ҫ������STATUS */
    /*����update statusΪEU2*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

    /*ɾ��GUTI,KSIasme,TAI list,GUTI*/
    NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

    /*������ɾ��TAI List������T3402,˳���ܵߵ�*/
    /*������ʱ��T3402*/
    NAS_LMM_StartPtlTimer(               TI_NAS_EMM_PTL_T3402);

    /*����attach attempt counter*/
    /*NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;*/

    /*�޸�״̬��������״̬DEREG��״̬DEREG_ATTEMPTING_TO_ATTACH*/
    NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                        EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                        TI_NAS_EMM_STATE_NO_TIMER);


}


VOS_VOID    NAS_EMM_AttSendEsmResult( EMM_ESM_ATTACH_STATUS_ENUM_UINT32 attStatus )
{
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_AttSendEsmResult is entered");

    /*�жϴ���ԭ��*/
    if (EMM_ATTACH_CAUSE_ESM == NAS_EMM_GLO_AD_GetAttCau())
    {
        /*��ESM����ID_EMM_ESM_EST_CNF��Ϣ*/
        NAS_EMM_EsmSendEstCnf(          attStatus);

        if (EMM_ESM_ATTACH_STATUS_DETACHED == attStatus)
        {
            /*clear global:EpsContextStatusChange*/
            NAS_EMM_GetEpsContextStatusChange() = NAS_EMM_EPS_BEARER_STATUS_NOCHANGE;
        }
    }
    else
    {
        /*��ESM����ID_EMM_ESM_STATUS_IND��Ϣ*/
        NAS_EMM_EsmSendStatResult(      attStatus);
    }

    return;
}


VOS_VOID  NAS_EMM_UpdateCnAttTypeOfUserReq( VOS_VOID )
{
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_UpdateCnAttTypeOfUserReq is entered");

    /* �ж��Ƿ�ԭ��ֵ2�ܾ��� */
    if (NAS_EMM_REJ_YES == NAS_LMM_GetEmmInfoRejCause2Flag())
    {
        NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);
        return ;
    }

    /* �ж��Ƿ�ԭ��ֵ18�ܾ��� */
    if (NAS_EMM_REJ_YES == NAS_EMMC_GetRejCause18Flag())
    {
        NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);
        return ;
    }

    /*�û��·�CS_ONLY,��UEģʽΪPS1,PS2��CS/PS1,CS/PS2,���յ��û�������Ϣʱ��ظ���
    �˴�������*/
    if (NAS_EMM_SUCC == NAS_EMM_CheckMutiModeSupport())
    {
        if(MMC_LMM_ATT_REQ_TYPE_PS_ONLY == NAS_EMM_GLO_GetAttReqType())
        {
            NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);
            return;
        }
        if ((MMC_LMM_ATT_REQ_TYPE_CS_PS == NAS_EMM_GLO_GetAttReqType()) &&
            (NAS_EMM_YES== NAS_EMM_IsCsPsUeMode()))
        {

            NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI);
        }
        else
        {
            NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);
        }
    }
    else
    {
        /*PC REPLAY MODIFY BY LEILI BEGIN*/
        /*L��ģ��,����ģʽCS+PS���û��·�PS_ONLY��CS+PS��CS����ע��,��������ATTACH,
        �����������PS ATTACH*/
        if((NAS_EMM_YES== NAS_EMM_IsCsPsUeMode()) &&
            (NAS_EMM_AUTO_ATTACH_ALLOW == NAS_EMM_GetCsAttachAllowFlg()))
        {
            NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI);

        }
        else
        {
            NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);

        }
        /*PC REPLAY MODIFY BY LEILI END*/
    }
    return;
}
VOS_VOID  NAS_EMM_UpdateCnAttTypeOfNotUserReq( VOS_VOID )
{
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_UpdateCnAttTypeOfNotUserReq is entered");

    /* �ж��Ƿ�ԭ��ֵ2�ܾ��� */
    if (NAS_EMM_REJ_YES == NAS_LMM_GetEmmInfoRejCause2Flag())
    {
        NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);
        return ;
    }

    /* �ж��Ƿ�ԭ��ֵ18�ܾ��� */
    if (NAS_EMM_REJ_YES == NAS_EMMC_GetRejCause18Flag())
    {
        NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);
        return ;
    }
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
     /* �ж��Ƿ�����CS��ע�� */
    if (NAS_EMM_AUTO_ATTACH_NOT_ALLOW == NAS_EMM_GetCsAttachAllowFlg())
    {
        NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);
        return ;
    }
    /*PC REPLAY MODIFY BY LEILI END*/
    if ((NAS_LMM_UE_CS_PS_MODE_1 == NAS_LMM_GetEmmInfoUeOperationMode()) ||
        (NAS_LMM_UE_CS_PS_MODE_2 == NAS_LMM_GetEmmInfoUeOperationMode()))
    {
        NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI);
    }
    else
    {
        NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_ONLY);
    }
    return;
}
VOS_VOID  NAS_EMM_SetAttType( VOS_VOID )
{
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_SetAttType is entered");

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_GLO_SetCnAttReqType(MMC_LMM_ATT_TYPE_EPS_EMERGENCY);
        return ;
    }

    if (NAS_EMM_OK == NAS_EMM_CheckAppMsgPara(ID_MMC_LMM_ATTACH_REQ))
    {
        NAS_EMM_UpdateCnAttTypeOfUserReq();
    }
    else
    {
        NAS_EMM_UpdateCnAttTypeOfNotUserReq();
    }
    return;
}


VOS_VOID  NAS_EMM_SetUeUsageSetting (VOS_VOID )
{
    NAS_EMM_ATTACH_LOG1_INFO("NAS_EMM_SetUeUsageSetting: ",
                             NAS_EMM_GLO_AD_GetUsgSetting());

    /* ��һ�׶Σ�ֻ֧���������ģ�����ʱ�Ѿ�����ֵ���˴����ٱ仯 */

    /*
    if ((NAS_LMM_UE_CS_PS_MODE_1 == NAS_LMM_GetEmmInfoUeOperationMode()) ||
        (NAS_LMM_UE_PS_MODE_1 == NAS_LMM_GetEmmInfoUeOperationMode()))
    {
        NAS_EMM_GLO_AD_SetUsgSetting(EMM_SETTING_VOICE_CENTRIC);
    }
    else
    {
        NAS_EMM_GLO_AD_SetUsgSetting(EMM_SETTING_DATA_CENTRIC);
    }
    */
    return;
}


VOS_UINT32  NAS_EMM_GetCsService (VOS_VOID )
{
    NAS_EMM_PUB_INFO_STRU                  *pstPubInfo      = VOS_NULL_PTR;
    NAS_LMM_CS_SERVICE_ENUM_UINT32          ulCsService     = NAS_LMM_CS_SERVICE_BUTT;
    /* lihong00150010 csfb begin */

    NAS_MML_LTE_CS_SERVICE_CFG_ENUM_UINT8   enCsServiceCfg  = NAS_MML_LTE_SUPPORT_BUTT;
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    enCsServiceCfg  = NAS_EMM_GetLteCsServiceCfg();
    /*PC REPLAY MODIFY BY LEILI END*/
    NAS_EMM_ATTACH_LOG1_INFO("MML CS SERVICE: ", enCsServiceCfg);

    pstPubInfo      = NAS_LMM_GetEmmInfoAddr();

    if (NAS_MML_LTE_SUPPORT_BUTT != enCsServiceCfg)
    {
        pstPubInfo->bitOpCsService  = NAS_EMM_BIT_SLCT;

        pstPubInfo->ulCsService     = enCsServiceCfg;
        /*pstPubInfo->ulCsService = NAS_LMM_ChangeCsServiceMmlToLmm(enCsServiceCfg);*/
    }
    else
    {
        pstPubInfo->bitOpCsService = NAS_EMM_BIT_NO_SLCT;
    }

    /* lihong00150010 csfb end */
    if (NAS_EMM_BIT_SLCT == pstPubInfo->bitOpCsService)
    {

        ulCsService = pstPubInfo->ulCsService;
    }

    return ulCsService;
}
VOS_UINT32  NAS_EMM_GetVoiceDomain(VOS_VOID )
{
    NAS_EMM_PUB_INFO_STRU               *pstPubInfo;
    NAS_LMM_VOICE_DOMAIN_ENUM_UINT32    ulVoiceDomain = NAS_LMM_VOICE_DOMAIN_BUTT;

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();

    if (NAS_EMM_BIT_SLCT == pstPubInfo->bitOpVoiceDomain)
    {

        ulVoiceDomain = pstPubInfo->ulVoiceDomain;
    }

    return ulVoiceDomain;
}



VOS_UINT32  NAS_EMM_GetSuppSrvccToGU(VOS_VOID )
{
    NAS_EMM_PUB_INFO_STRU               *pstPubInfo;
    NAS_EMM_MS_NET_CAP_STRU             *pstMsNetCap;
    VOS_UINT32                          ulRslt = NAS_EMM_BIT_NO_SLCT;

    pstPubInfo = NAS_LMM_GetEmmInfoAddr();
    pstMsNetCap = NAS_LMM_GetEmmInfoMsNetCapAddr();
    if (NAS_EMM_BIT_SLCT == pstPubInfo->bitOpMsNetCap)
    {
        if (NAS_EMM_BIT_SLCT == ((pstMsNetCap->aucMsNetCap[2] >> NAS_EMM_AD_MOVEMENT_3_BITS)
                                    & NAS_EMM_LOW_1_BITS_F))
        {
            ulRslt = NAS_EMM_BIT_SLCT;
        }

    }

    return ulRslt;
}


VOS_VOID  NAS_EMM_ProcCause161722TauAttemptCont(VOS_VOID )
{
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg = NAS_EMM_NULL_PTR;

    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_ProcCause161722TauAttemptCont is entered");

    if (NAS_EMM_TAU_ATTEMPT_CNT_MAX > NAS_EMM_TAU_GetEmmTAUAttemptCnt())
    {
        /*������ʱ��TI_NAS_EMM_PTL_T3411*/
        NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3411);

		/* lihong00150010 emergency delete */

        /*�޸�״̬��������״̬REG��״̬ATTACH_WAIT_ESM_BEARER_CNF*/
        NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                        EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*ATTACH�����MMC����ATTACH�ɹ�*/
        NAS_EMM_AppSendAttSucc();

        pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
        if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
            && (NAS_EMM_NULL_PTR != pstEsmMsg))
        {
            /*��ESM����ID_EMM_ESM_EST_CNF��Ϣ*/
            NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);

            NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_OTHER;
            NAS_EMM_ClearEmcEsmMsg();
        }
    }
    else
    {
        /*������ʱ��TI_NAS_EMM_T3402*/
        NAS_LMM_StartPtlTimer(      TI_NAS_EMM_PTL_T3402);
		
		/* lihong00150010 emergency delete */

        /*�޸�״̬��������״̬REG��״̬EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM*/
        NAS_EMM_AdStateConvert(     EMM_MS_REG,
                                    EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM,
                                    TI_NAS_EMM_PTL_T3402);

        /*ATTACH�����MMC����ATTACH�ɹ�*/
        NAS_EMM_AppSendAttSucc();
        /* ʶ��EPS ONLY,ԭ��ֵΪ16,17,22 disable Lģ�ĳ���,�ͷ���· */
        /*NAS_EMM_RecogAndProc161722Disable(NAS_EMM_GLO_AD_GetEmmCau());*/
        /*NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);*/
    }

    return;
}

/*****************************************************************************
 Function Name   : NAS_EMM_SetAttachRegDomain
 Description     : ATTACH�ɹ�ʱ����ע����
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2011-08-26  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_SetAttachRegDomain( VOS_VOID )
{
    if (EMM_ATTACH_RST_PS == NAS_EMM_GLO_AD_GetAttRslt())
    {
        NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
    }
    else
    {
        NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_CS_PS);
    }
}

/*****************************************************************************
 Function Name   : NAS_EMM_ProcRrcDataCnfEpsSuccOnly
 Description     : �յ�RRC_MM_DATA_CNF���ҽ��ΪEPS ONLY�ɹ��Ĵ���
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2011-09-26  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_ProcRrcDataCnfEpsSuccOnly( VOS_VOID )
{
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg = NAS_EMM_NULL_PTR;

    if (NAS_LMM_CAUSE_IMSI_UNKNOWN_IN_HSS == NAS_EMM_GLO_AD_GetEmmCau())
    {
        /* ��¼UE��ԭ��ֵ2�ܾ�����֮���ٷ�������ATTACH������TAU��ֱ���ػ����߰ο� */
        NAS_LMM_SetEmmInfoRejCause2Flag(NAS_EMM_REJ_YES);

        /*�޸�״̬��������״̬REG��״̬REG_NORMAL_SERVICE*/
        NAS_EMM_AdStateConvert(     EMM_MS_REG,
                                    EMM_SS_REG_NORMAL_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        /*ATTACH�����MMC����ATTACH�ɹ�*/
        NAS_EMM_AppSendAttSucc();

        pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
        if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
            && (NAS_EMM_NULL_PTR != pstEsmMsg))
        {
            /*��ESM����ID_EMM_ESM_EST_CNF��Ϣ*/
            NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);

            NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_OTHER;
            NAS_EMM_ClearEmcEsmMsg();
        }

        return ;
    }


    if (NAS_LMM_CAUSE_CS_NOT_AVAIL == NAS_EMM_GLO_AD_GetEmmCau())
    {
        /*�ѵ�ǰPLMN���뵽�ܾ�#18�б���*/
        NAS_EMMC_AddPlmnInRej18PlmnList(NAS_LMM_GetEmmInfoPresentPlmnAddr());

        /*���þܾ�18��ʶ*/
        NAS_EMMC_SetRejCause18Flag(NAS_EMM_REJ_YES);

        /*�޸�״̬��������״̬REG��״̬REG_NORMAL_SERVICE*/
        NAS_EMM_AdStateConvert(     EMM_MS_REG,
                                    EMM_SS_REG_NORMAL_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        /*ATTACH�����MMC����ATTACH�ɹ�*/
        NAS_EMM_AppSendAttSucc();

        pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
        if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
            && (NAS_EMM_NULL_PTR != pstEsmMsg))
        {
            /*��ESM����ID_EMM_ESM_EST_CNF��Ϣ*/
            NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);

            NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_OTHER;
            NAS_EMM_ClearEmcEsmMsg();
        }

        /* ʶ��EPS ONLY,ԭ��ֵΪ18 disable Lģ�ĳ���,�ͷ���· */
        /*NAS_EMM_RecogAndProc18Disable(NAS_EMM_GLO_AD_GetAttRejCau());*/
        return;

    }

    NAS_EMM_ProcCause161722TauAttemptCont();

    return;

    #if 0
    if ((NAS_LMM_CAUSE_MSC_UNREACHABLE == NAS_EMM_GLO_AD_GetAttRejCau()) ||
        (NAS_LMM_CAUSE_NETWORK_FAILURE == NAS_EMM_GLO_AD_GetAttRejCau()) ||
        (NAS_LMM_CAUSE_PROCEDURE_CONGESTION == NAS_EMM_GLO_AD_GetAttRejCau()))
    {
        NAS_EMM_ProcCause161722TauAttemptCont();
        return ;
    }

    /*�ѵ�ǰPLMN���뵽�ܾ�#18�б���*/
    NAS_EMMC_AddPlmnInRej18PlmnList(NAS_LMM_GetEmmInfoPresentPlmnAddr());

    /*���þܾ�18��ʶ*/
    NAS_EMMC_SetRejCause18Flag(NAS_EMM_REJ_YES);

    /*�޸�״̬��������״̬REG��״̬REG_NORMAL_SERVICE*/
    NAS_EMM_AdStateConvert(     EMM_MS_REG,
                                EMM_SS_REG_NORMAL_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*ATTACH�����MMC����ATTACH�ɹ�*/
    NAS_EMM_AppSendAttSucc();

    pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
    if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        && (NAS_EMM_NULL_PTR != pstEsmMsg))
    {
        /*��ESM����ID_EMM_ESM_EST_CNF��Ϣ*/
        NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);

        NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_OTHER;
        NAS_EMM_ClearEmcEsmMsg();
    }

    /* ʶ��EPS ONLY,ԭ��ֵΪ18 disable Lģ�ĳ���,�ͷ���· */
    /*NAS_EMM_RecogAndProc18Disable(NAS_EMM_GLO_AD_GetAttRejCau());*/
    #endif

}


VOS_VOID  NAS_EMM_ProcRrcDataCnfSucc( VOS_VOID )
{
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg = NAS_EMM_NULL_PTR;

    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_ProcRrcDataCnfSucc is entered");

    /*���ݴ���ԭ����ESM���ͽ��*/
    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_ATTACHED);

    /*����UEָ���� Drx_Cycle��RRC*/

    if(NAS_EMM_DRX_CYCLE_LEN_CHANGED == NAS_EMM_GetDrxCycleLenChangeFlag())
    {
        NAS_EMM_SendUeSpecDrxCycleLenToRrc();
        NAS_EMM_SetDrxCycleLenChangeFlag(NAS_EMM_DRX_CYCLE_LEN_NOT_CHANGED);
    }

    NAS_MML_SetPsRegContainDrx(NAS_MML_LTE_PS_REG_CONTAIN_DRX_PARA);

    NAS_EMM_GLO_AD_GetAttAtmpCnt() = 0;
    /*leili modify for isr begin*/
    /*���� LMM_MMC_TIN_TYPE_IND*/
    NAS_EMM_SetTinType(MMC_LMM_TIN_GUTI);
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /*�������г��ص�ISR��ʶΪISR����ǰ*/
    NAS_EMM_UpdateBearISRFlag(NAS_EMM_GetPsBearerCtx());
    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_ProcRrcDataCnfSucc: UPDATE MML PS BEARER INFO:");
    NAS_COMM_PrintArray(            NAS_COMM_GET_MM_PRINT_BUF(),
                                    (VOS_UINT8*)NAS_MML_GetPsBearerCtx(),
                                    sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)
                                    *EMM_ESM_MAX_EPS_BEARER_NUM);
    /*PC REPLAY MODIFY BY LEILI END*/
    /*leili modify for isr end*/
    NAS_EMM_SetAttachRegDomain();

    if ((MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI == NAS_EMM_GLO_GetCnAttReqType()) &&
        (EMM_ATTACH_RST_PS == NAS_EMM_GLO_AD_GetAttRslt()))
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_ProcRrcDataCnfSucc Eps Only");

        NAS_EMM_ProcRrcDataCnfEpsSuccOnly();
    }
    else
    {
        /*�޸�״̬��������״̬REG��״̬REG_NORMAL_SERVICE*/
        if (NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
        {
            NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                            EMM_SS_REG_LIMITED_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

            NAS_LMM_SetEmmInfoRegStatus(NAS_LMM_REG_STATUS_EMC_REGED);
        }
        else
        {
            NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                            EMM_SS_REG_NORMAL_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

            NAS_LMM_SetEmmInfoRegStatus(NAS_LMM_REG_STATUS_NORM_REGED);
        }

        pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
        if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
            && (NAS_EMM_NULL_PTR != pstEsmMsg))
        {
            /*��ESM����ID_EMM_ESM_EST_CNF��Ϣ*/
            NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);

            NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_OTHER;
            NAS_EMM_ClearEmcEsmMsg();
        }

        /*ATTACH�����MMC����ATTACH�ɹ�*/
        NAS_EMM_AppSendAttSucc();

        /* ʶ��SMS only����CS fallback not preferred disable Lģ�ĳ���,�ͷ���· */
        /*NAS_EMM_RecogAndProcSmsOnlyCsfbNotPrefDisable();*/
    }



    /* ATTACH �ɹ������֮ǰ��������ϵͳ�任����������������ͼ�¼��Ϣ */
    NAS_EMM_ClearResumeInfo();

    return;
}

VOS_VOID  NAS_EMM_AttProcT3410Exp( VOS_VOID )
{
    if (NAS_EMM_GLO_AD_GetAttAtmpCnt()  < 5)
    {
        NAS_EMM_AtmpCntLess5Proc();
        NAS_EMM_AppSendAttTimerExp();
    }
    else
    {
        NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

        /*ɾ��GUTI,KSIasme,TAI list,GUTI*/
        NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

        /*������ɾ��TAI List������T3402,˳���ܵߵ�*/
        /*������ʱ��T3402*/
        NAS_LMM_StartPtlTimer(               TI_NAS_EMM_PTL_T3402);



        /*�޸�״̬��������״̬DEREG��״̬DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        /* ע��������ϱ���������attach attempt counter */
        NAS_EMM_AppSendAttTimerExp();

        /*����attach attempt counter*/
        /*NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;*/
    }
}
VOS_VOID  NAS_EMM_AttProcEpsOnlyRejValueOther( VOS_VOID )
{
    if (NAS_EMM_GLO_AD_GetAttAtmpCnt()  < 5)
    {
        NAS_EMM_AtmpCntLess5Proc();
        NAS_EMM_AppSendAttRejEpsOnly();
    }
    else
    {
        NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

        /*ɾ��GUTI,KSIasme,TAI list,GUTI*/
        NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

        /*������ɾ��TAI List������T3402,˳���ܵߵ�*/
        /*������ʱ��T3402*/
        NAS_LMM_StartPtlTimer(               TI_NAS_EMM_PTL_T3402);



        /*�޸�״̬��������״̬DEREG��״̬DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        /* ע��������ϱ���������attach attempt counter */
        NAS_EMM_AppSendAttRejEpsOnly();

        /*����attach attempt counter*/
        /*NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;*/
    }

    /*��MRRC����NAS_EMM_MRRC_REL_REQ��Ϣ*/
    NAS_EMM_RelReq( NAS_LMM_NOT_BARRED);
}

VOS_VOID  NAS_EMM_ProcRejCauOtherAbnormal( VOS_VOID )
{
    if (NAS_EMM_GLO_AD_GetAttAtmpCnt()  < 5)
    {
        NAS_EMM_AtmpCntLess5Proc();
        /* ���ձ��ʵ�֣���ATTACH������ԭ��ֵ�ܾ������Դ���С��5ʱ������Ч��ȫ������д�� */
        NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);
        NAS_EMM_AppSendAttRej();
    }
    else
    {
        NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

        /*ɾ��GUTI,KSIasme,TAI list,GUTI*/
        NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

        /*������ɾ��TAI List������T3402,˳���ܵߵ�*/
        /*������ʱ��T3402*/
        NAS_LMM_StartPtlTimer(               TI_NAS_EMM_PTL_T3402);



        /*�޸�״̬��������״̬DEREG��״̬DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        /* ע��������ϱ���������attach attempt counter */
        NAS_EMM_AppSendAttRej();

        /*����attach attempt counter*/
        /*NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;*/
    }
}


VOS_VOID  NAS_EMM_AD_Reattach(VOS_VOID )
{
    NAS_EMM_ATTACH_LOG_INFO(            "NAS_EMM_AD_Reattach entered!");

    /*֪ͨESM*/
    NAS_EMM_AttSendEsmResult(           EMM_ESM_ATTACH_STATUS_DETACHED);

    /*����ATTACH*/
    /*NAS_EMM_EsmSendPdnConnInd(          NAS_EMM_GLO_AD_GetDefBeaId());*/
    NAS_EMM_EsmSendPdnConnInd(          0);
    NAS_LMM_StartStateTimer(             TI_NAS_EMM_WAIT_ESM_PDN_RSP);
    NAS_EMM_AdStateConvert(             EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_ESM_PDN_RSP,
                                        TI_NAS_EMM_WAIT_ESM_PDN_RSP);
   return;
}
/*****************************************************************************
 Function Name   : NAS_EMM_Attach_IsTimerRunning
 Description     : �ж�3411��3402��ʱ���Ƿ�������
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.FTY         2012-02-24  Draft Enact
*****************************************************************************/
VOS_UINT32 NAS_EMM_Attach_IsTimerRunning(VOS_VOID )
{
    /* T3411�����У�����MMC��ATTACH���*/
    if (NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3411))
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_Attach_IsTimerRunning: T3411 is running.");

        NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                    EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        return NAS_EMM_YES;
    }

    /* T3402���У���MMC��ATTACH���������MMC����һ������*/
    if (NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3402))
    {
         NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_Attach_IsTimerRunning: T3402 is running.");

        NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                    EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        /*��MMC����LMM_MMC_ATTACH_CNF��LMM_MMC_ATTACH_IND��Ϣ*/
        NAS_EMM_AppSendAttRstDefaultReqType(MMC_LMM_ATT_RSLT_T3402_RUNNING);
        return NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}
VOS_VOID  NAS_EMM_ProcAttachConnFailOtherCause( VOS_VOID )
{
    if (NAS_EMM_GLO_AD_GetAttAtmpCnt()  < 5)
    {
        /*������ʱ��T3411*/
        NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3411);

        /*�޸�״̬��������״̬DEREG��״̬DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                            TI_NAS_EMM_STATE_NO_TIMER);
        NAS_EMM_AppSendAttFailWithPara();

    }

    else
    {
        /* ɾ��GUTI����Զ�����EPS_LOC,������Ҫ������STATUS */
        /*����update statusΪEU2*/
        NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

        /*ɾ��GUTI,KSIasme,TAI list,GUTI*/
        NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

        /*������ɾ��TAI List������T3402,˳���ܵߵ�*/
        /*������ʱ��T3402*/
        NAS_LMM_StartPtlTimer(               TI_NAS_EMM_PTL_T3402);

        /* ������ת״̬�����ϱ�ATTACH����������� attach attempt counter,
            ˳���ܱ� */

        /*�޸�״̬��������״̬DEREG��״̬DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        NAS_EMM_AppSendAttFailWithPara();

        /*����attach attempt counter*/
        /*NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;*/
    }
}
VOS_VOID  NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailBarredProc( VOS_VOID )
{
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg = NAS_EMM_NULL_PTR;


    pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
    if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        && (NAS_EMM_NULL_PTR != pstEsmMsg))
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailBarredProc: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttOtherType(MMC_LMM_ATT_RSLT_ACCESS_BAR);

        /*����attach attempt counter��˳��������ϱ�ATTACH���֮��*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

        /* ֪ͨESM������EMM��Ҫ������ע�ᣬESM����Ҫ�����̬���еĽ���PDN������¼ */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        /*send INTRA_CONN2IDLE_REQ����������״̬*/
        NAS_EMM_CommProcConn2Ilde();

        /*����ESM��Ϣ*/
        NAS_LMM_MEM_CPY(        NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                               &(pstEsmMsg->stEsmMsg),
                                (pstEsmMsg->stEsmMsg.ulEsmMsgSize)+4);

        NAS_EMM_StartEmergencyAttach();

        return ;
    }

    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_ATTACH_NEEDED,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*��MMC����ATTACH���뱻bar*/
    NAS_EMM_AppSendAttOtherType(MMC_LMM_ATT_RSLT_ACCESS_BAR);

    /*send INTRA_CONN2IDLE_REQ����������״̬*/
    NAS_EMM_CommProcConn2Ilde();

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* ֪ͨIMSA�ٳ���CS�� */
    }

    return ;
}
VOS_VOID  NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailCellSrchProc( VOS_VOID )
{
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg = NAS_EMM_NULL_PTR;

    pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
    if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        && (NAS_EMM_NULL_PTR != pstEsmMsg))
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailOthersProc: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttFailWithPara();

        /*����attach attempt counter��˳��������ϱ�ATTACH���֮��*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

        /* ֪ͨESM������EMM��Ҫ������ע�ᣬESM����Ҫ�����̬���еĽ���PDN������¼ */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        /* ״̬Ǩ��Dereg.Plmn_Search*/
        NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                    EMM_SS_DEREG_PLMN_SEARCH,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        /*send INTRA_CONN2IDLE_REQ����������״̬*/
        NAS_EMM_CommProcConn2Ilde();

        return ;
    }

    /* ֪ͨESM,APPע��ʧ��*/
    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_DETACHED);

    /* ״̬Ǩ��Dereg.Plmn_Search*/
    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_PLMN_SEARCH,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*send INTRA_CONN2IDLE_REQ����������״̬*/
    NAS_EMM_CommProcConn2Ilde();

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* ֪ͨIMSA�ٳ���CS�� */
    }

    return ;
}



VOS_VOID  NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailOthersProc( VOS_VOID )
{
    NAS_EMM_ESM_MSG_BUFF_STRU          *pstEsmMsg = NAS_EMM_NULL_PTR;

    NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

    pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
    if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        && (NAS_EMM_NULL_PTR != pstEsmMsg))
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailOthersProc: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttFailWithPara();

        /*����attach attempt counter��˳��������ϱ�ATTACH���֮��*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

        /* ֪ͨESM������EMM��Ҫ������ע�ᣬESM����Ҫ�����̬���еĽ���PDN������¼ */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        /*send INTRA_CONN2IDLE_REQ����������״̬*/
        NAS_EMM_CommProcConn2Ilde();

        /*����ESM��Ϣ*/
        NAS_LMM_MEM_CPY(        NAS_EMM_GLO_AD_GetEsmMsgAddr(),
                               &(pstEsmMsg->stEsmMsg),
                                (pstEsmMsg->stEsmMsg.ulEsmMsgSize)+4);

        NAS_EMM_StartEmergencyAttach();

        return ;
    }

    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_ProcAttachConnFailOtherCause();

    /*send INTRA_CONN2IDLE_REQ����������״̬*/
    NAS_EMM_CommProcConn2Ilde();

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* ֪ͨIMSA�ٳ���CS�� */
    }

    return ;
}


VOS_UINT32  NAS_EMM_MsRegInitSsWtCnAttCnfMsgIntraConncetFailInd(
                                                    VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru )
{
    NAS_EMM_MRRC_CONNECT_FAIL_IND_STRU         *pMrrcConnectFailInd = NAS_EMM_NULL_PTR;

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtCnAttCnfMsgIntraConncetFailInd is entered");

    /*check the current state*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF))
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtCnAttCnfMsgIntraConncetFailInd: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

     /*check the input ptr*/
    if (NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtCnAttCnfMsgIntraConncetFailInd: NULL PTR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*stop T3410*/
    NAS_LMM_StopStateTimer(          TI_NAS_EMM_T3410);

    /*get the msg*/
    pMrrcConnectFailInd = (NAS_EMM_MRRC_CONNECT_FAIL_IND_STRU *)pMsgStru;

    /*process different with the est result*/
    switch(pMrrcConnectFailInd->enEstResult)
    {
        case LRRC_EST_ACCESS_BARRED_MO_SIGNAL:
        case LRRC_EST_ACCESS_BARRED_ALL:

            NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailBarredProc();
            break;

        /* ����ʧ�ܣ�RRC���ڽ���С������*/
        case LRRC_EST_CELL_SEARCHING:

            NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailCellSrchProc();
            break;

        default:
            NAS_EMM_MsRegInitSsWtCnAttCnfIntraConnFailOthersProc();
            break;
    }

    return  NAS_LMM_MSG_HANDLED;
}


VOS_VOID  NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgAuthRej(
                                                    VOS_UINT32  ulCause)
{
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgAuthRej entered");

    (VOS_VOID)ulCause;

    /*ֹͣ��ʱ��T3410*/
    NAS_LMM_StopStateTimer(          TI_NAS_EMM_T3410);
    NAS_LMM_StopPtlTimer(                TI_NAS_EMM_PTL_T3416);
    NAS_EMM_SecuClearRandRes();

    NAS_EMM_AttDataUpdate(NAS_EMM_DELETE_RPLMN);

    NAS_LMM_SetPsSimValidity(NAS_LMM_SIM_INVALID);

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

        /* ֪ͨESM������EMM��Ҫ������ע�ᣬESM����Ҫ�����̬���еĽ���PDN������¼ */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
    }
    else
    {
        /*��ESM����ID_EMM_ESM_STATUS_IND��Ϣ*/
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);
    }

    /*�޸�״̬��������״̬DEREG��״̬DEREG_NO_IMSI*/
    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_NO_IMSI,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*��MMC����LMM_MMC_ATTCH_CNF��LMM_MMC_ATTCH_IND��Ϣ*/
    NAS_EMM_AppSendAttOtherType(MMC_LMM_ATT_RSLT_AUTH_REJ);

    /*��LRRC����LRRC_LMM_NAS_INFO_CHANGE_REQЯ��USIM��״̬*/
    NAS_EMM_SendUsimStatusToRrc(LRRC_LNAS_USIM_PRESENT_INVALID);

    /*��MRRC����NAS_EMM_MRRC_REL_REQ��Ϣ*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    return;
}
VOS_VOID  NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgRrcRelInd(VOS_UINT32 ulCause)
{
    /*��ӡ����ú���*/
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgRrcRelInd is entered");

    (VOS_VOID)(ulCause);

    /*ֹͣ��ʱ��T3410*/
    NAS_LMM_StopStateTimer(          TI_NAS_EMM_T3410);

    NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgRrcRelInd: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttFailWithPara();

        /*����attach attempt counter��˳�������ATTACH����ϱ�֮��*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

        /* ֪ͨESM������EMM��Ҫ������ע�ᣬESM����Ҫ�����̬���еĽ���PDN������¼ */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        NAS_EMM_AdStateConvert(          EMM_MS_DEREG,
                                         EMM_SS_DEREG_PLMN_SEARCH,
                                         TI_NAS_EMM_STATE_NO_TIMER);

        /*send INTRA_CONN2IDLE_REQ����������״̬*/
        NAS_EMM_CommProcConn2Ilde();

        return ;
    }

    if (NAS_EMM_GLO_AD_GetAttAtmpCnt()  < 5)
    {
        /*������ʱ��T3411*/
        NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3411);

        /*�޸�״̬��������״̬DEREG��״̬DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(          EMM_MS_DEREG,
                                         EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                         TI_NAS_EMM_STATE_NO_TIMER);

        NAS_EMM_AppSendAttFailWithPara();
    }

    else
    {
        /* ɾ��GUTI����Զ�����EPS_LOC,������Ҫ������STATUS */
        /*����update statusΪEU2*/
        NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

        /*ɾ��GUTI,KSIasme,TAI list,GUTI*/
        NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

        /*������ɾ��TAI List������T3402,˳���ܵߵ�*/
        /*������ʱ��T3402*/
        NAS_LMM_StartPtlTimer(               TI_NAS_EMM_PTL_T3402);

        /* ������ת״̬�����ϱ����������� attach attempt counter */

        /*�޸�״̬��������״̬DEREG��״̬DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        NAS_EMM_AppSendAttFailWithPara();

        /*����attach attempt counter*/
        /*NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;*/
    }

    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_DETACHED);

    /*send INTRA_CONN2IDLE_REQ����������״̬*/
    NAS_EMM_CommProcConn2Ilde();

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* ֪ͨIMSA�ٳ���CS�� */
    }

    return;

}



VOS_UINT32  NAS_EMM_MsRegInitSsWtCnAttCnfMsgAuthFail(
                                                    VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru )
{
    NAS_EMM_INTRA_AUTH_FAIL_STRU        *pMsgAuthFail   =   (NAS_EMM_INTRA_AUTH_FAIL_STRU *)pMsgStru;
    VOS_UINT32                          ulCause;

    /*��ӡ����ú���*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtCnAttCnfMsgAuthFail is entered");
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;


    /*���ԭ��ֵ*/
    ulCause                                       =         pMsgAuthFail->ulCause;

    /*����ԭ��ֵ����*/
    if(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL        ==        ulCause)
    {
        NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgAuthRej(        ulCause);
    }
    else
    {
        NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgRrcRelInd(      ulCause);
    }

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRegInitSsWtCnAttCnfMsgAuthRej(
                                        VOS_UINT32  ulMsgId,
                                        VOS_VOID   *pMsgStru )
{

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtCnAttCnfMsgAuthRej entered");
    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgAuthRej(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL);


    return  NAS_LMM_MSG_HANDLED;
}



VOS_UINT32  NAS_EMM_MsRegInitSsWtCnAttCnfMsgRrcRelInd(
                                                    VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru )
{
    LRRC_LMM_REL_IND_STRU     *pRrcRelInd =         (LRRC_LMM_REL_IND_STRU*)(pMsgStru);
    VOS_UINT32               ulCause;

    /*��ӡ����ú���*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtCnAttCnfMsgRrcRelInd is entered");

    (VOS_VOID)ulMsgId;

    /*���ԭ��ֵ*/
    ulCause                             =         pRrcRelInd->enRelCause;

    NAS_EMM_MsRegInitSsWtCnAttCnfProcMsgRrcRelInd(ulCause);

    return  NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsRegInitSsWtEsmPdnRspMsgRrcRelInd(
                                                    VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru )
{
    LRRC_LMM_REL_IND_STRU                *pRrcRelInd = VOS_NULL_PTR;

    (VOS_VOID)ulMsgId;

    /*RRC_REL_IND��ϢԤ�������ж��������IDLE̬����ֱ�Ӷ�������������һ����CONN*/

    /*CONN̬,�ͷ�����*/
    pRrcRelInd = (LRRC_LMM_REL_IND_STRU*)(pMsgStru);
    NAS_EMM_ATTACH_LOG1_INFO("NAS_EMM_MsRegInitSsWtEsmPdnRspMsgRrcRelInd Cause:", pRrcRelInd->enMsgId);
    NAS_EMM_CommProcConn2Ilde();

    return  NAS_LMM_MSG_HANDLED;

}


VOS_UINT32  NAS_EMM_MsRegInitSsWtCnAttCnfMsgT3410Exp(VOS_UINT32  ulMsgId,
                                                    const VOS_VOID   *pMsgStru )
{

    /*��ӡ����ú���*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtCnAttCnfMsgT3410Exp is entered");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*���״̬�Ƿ�ƥ�䣬����ƥ�䣬�˳�*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF))
    {
        /*��ӡ������Ϣ*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtCnAttCnfMsgT3410Exp: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*ATTACH���Լ�������1*/
    NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_MsRegInitSsWtCnAttCnfMsgT3410Exp: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttTimerExp();

        /*����attach attempt counter��˳��������ϱ�ATTACH���֮��*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()  = 0;

        /* ֪ͨESM������EMM��Ҫ������ע�ᣬESM����Ҫ�����̬���еĽ���PDN������¼ */
        NAS_EMM_AttSendEsmResult(       EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                        EMM_SS_DEREG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*��MRRC����NAS_EMM_MRRC_REL_REQ��Ϣ*/
        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);

        return  NAS_LMM_MSG_HANDLED;
    }

    /*��ESM����ATTACH���*/
    NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_AttProcT3410Exp();

    /*��MRRC����NAS_EMM_MRRC_REL_REQ��Ϣ*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* ֪ͨIMSA�ٳ���CS�� */
    }

    return  NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_EMM_MsDrgSsAttemptToAttMsgT3411Exp( VOS_UINT32  ulMsgId,
                                                    const VOS_VOID   *pMsgStru  )
{

    /*��ӡ����ú���*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsDrgSsAttemptToAttMsgT3411Exp is entered");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*���״̬�Ƿ�ƥ�䣬����ƥ�䣬�˳�*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG, EMM_SS_DEREG_ATTEMPTING_TO_ATTACH))
    {
        /*��ӡ������Ϣ*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDrgSsAttemptToAttMsgT3411Exp: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }


    #if 1
    /*����ATTATCH*/
    NAS_EMM_AD_Reattach();

    #else
    /*���津��ATTACH��ԭ��*/
    /*NAS_EMM_GLO_AD_GetAttCau()          = EMM_ATTACH_CAUSE_TIMER_EXP;*/

    /*��MRRC����ATTACH REQUEST��Ϣ*/
    NAS_EMM_MrrcSendAttReq(             NAS_EMM_GLO_AD_GetEsmMsgLen());

    /*������ʱ��T3410*/
    NAS_LMM_StartStateTimer(         TI_NAS_EMM_T3410);

    /*�޸�״̬��������״̬REG_INIT��״̬ATTACH_WAIT_CN_ATTACH_CNF*/
    NAS_EMM_AdStateConvert(             EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF,
                                        TI_NAS_EMM_T3410);
    #endif

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsDrgSsAttemptToAttMsgT3402Exp( VOS_UINT32  ulMsgId,
                                                    const VOS_VOID   *pMsgStru  )
{
    /*��ӡ����ú���*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsDrgSsAttemptToAttMsgT3402Exp is entered");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*���״̬�Ƿ�ƥ�䣬����ƥ�䣬�˳�*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG,EMM_SS_DEREG_ATTEMPTING_TO_ATTACH))
    {
        /*��ӡ������Ϣ*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDrgSsAttemptToAttMsgT3402Exp: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*��ESM����ID_EMM_ESM_PDN_CON_IND��Ϣ*/
    /*NAS_EMM_EsmSendPdnConnInd(          NAS_EMM_GLO_AD_GetDefBeaId());*/
    NAS_EMM_EsmSendPdnConnInd(          0);

    /*������ʱ��TI_NAS_EMM_WAIT_ESM_PDN_RSP*/
    NAS_LMM_StartStateTimer(         TI_NAS_EMM_WAIT_ESM_PDN_RSP);

    /*�޸�״̬��������״̬REG_INIT��״̬ATTACH_WAIT_ESM_PDN_RSP*/
    NAS_EMM_AdStateConvert(             EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_ESM_PDN_RSP,
                                        TI_NAS_EMM_WAIT_ESM_PDN_RSP);


    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32    NAS_EMM_MsDeregSsAttachNeededMsgRrcAccessGrantInd(VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru )
{
    LRRC_LMM_ACCESS_GRANT_IND_STRU           *pRrcMmGrantInd = NAS_EMM_NULL_PTR;
    VOS_UINT32                              ulRst;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsDeregSsAttachNeededMsgRrcAccessGrantInd is entered.");

    (VOS_VOID)ulMsgId;

    /*check the input ptr*/
    if (NAS_EMM_NULL_PTR == pMsgStru)
    {

        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDeregSsAttachNeededMsgRrcAccessGrantInd: NULL PTR!!!");
        return NAS_LMM_MSG_DISCARD;
    }

    /*check the current state*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG,EMM_SS_DEREG_ATTACH_NEEDED))
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDeregSsAttachNeededMsgRrcAccessGrantInd: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*get the grant msg*/
    pRrcMmGrantInd = (LRRC_LMM_ACCESS_GRANT_IND_STRU *)pMsgStru;
    if ((LRRC_LNAS_ACCESS_GRANT_MO_SIGNAL == pRrcMmGrantInd->enAccessGrantType) ||
        (LRRC_LNAS_ACCESS_GRANT_MO_SIGNAL_AND_MT == pRrcMmGrantInd->enAccessGrantType) ||
        (LRRC_LNAS_ACCESS_GRANT_MO_CALL_AND_MO_SIGNAL == pRrcMmGrantInd->enAccessGrantType)||
        (LRRC_LNAS_ACCESS_GRANT_MO_CALL_AND_MO_SIGNAL_AND_MT == pRrcMmGrantInd->enAccessGrantType)||
        (LRRC_LNAS_ACCESS_GRANT_MO_CALL_AND_MO_SIGNAL_AND_MO_CSFB == pRrcMmGrantInd->enAccessGrantType)||
        (LRRC_LNAS_ACCESS_GRANT_MO_SIGNAL_AND_MO_CSFB == pRrcMmGrantInd->enAccessGrantType)||
        (LRRC_LNAS_ACCESS_GRANT_MO_SIGNAL_AND_MO_CSFB_AND_MT == pRrcMmGrantInd->enAccessGrantType)||
        (LRRC_LNAS_ACCESS_GRANT_ALL == pRrcMmGrantInd->enAccessGrantType))
    {
        /*���津��ATTACH��ԭ��*/
        /*NAS_EMM_GLO_AD_GetAttCau()          = EMM_ATTACH_CAUSE_OTHER;*/

        /*����EMM_ESM_STATUS_IND(ȥע��)*/
        NAS_EMM_AttSendEsmResult(       EMM_ESM_ATTACH_STATUS_DETACHED);

        /*ת��EMM״̬��MS_DEREG+SS_DEREG_NORMAL_SERVICE*/
        NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                        EMM_SS_DEREG_NORMAL_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*����ATTACH REQ�ڲ���Ϣ��ATTACHģ��*/
        ulRst = NAS_EMM_SendIntraAttachReq();
        if(NAS_EMM_SUCC != ulRst)
        {
            NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_MsDeregSsAttachNeededMsgRrcAccessGrantInd: send INTRA ATTACH REQ ERR !");
        }
    }
    else
    {
        return NAS_LMM_MSG_DISCARD;
    }

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsDeregSsAttachNeededMsgMmcSysInfoInd( VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru )
{
    EMMC_EMM_SYS_INFO_IND_STRU          *pstsysinfo = NAS_EMM_NULL_PTR;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsDeregSsAttachNeededMsgMmcSysInfoInd entered.");

    (VOS_VOID)ulMsgId;

    /* ��μ��*/
    if (NAS_EMM_NULL_PTR == pMsgStru)
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDeregSsAttachNeededMsgMmcSysInfoInd: NULL PTR!!!");
        return NAS_LMM_MSG_DISCARD;
    }

    /* EMM״̬���*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG,EMM_SS_DEREG_ATTACH_NEEDED))
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDeregSsAttachNeededMsgMmcSysInfoInd: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /* ���ϵͳ��Ϣ��������Ǩ�Ƶ� Dereg.Limite_Service*/
    pstsysinfo                          = (EMMC_EMM_SYS_INFO_IND_STRU*)pMsgStru;

    if((EMMC_EMM_NO_FORBIDDEN != pstsysinfo->ulForbiddenInfo)
        ||(EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus))
    {
        NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                        EMM_SS_DEREG_LIMITED_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        NAS_EMM_SetAttType();

        NAS_EMM_AppSendAttRsltForb(pstsysinfo->ulForbiddenInfo);
    }

    return NAS_LMM_MSG_HANDLED;

}
VOS_UINT32    NAS_EMM_MsRegInitSsWtEsmPdnRspMsgTimerWtEsmRspExp(VOS_UINT32 ulMsgId,
                                                            const VOS_VOID * pMsgStru)

{
     /*��ӡ����ú���*/
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_MsRegInitSsWtEsmPdnRspMsgTimerWtEsmRspExp is entered");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*���״̬�Ƿ�ƥ�䣬����ƥ�䣬�˳�*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_ESM_PDN_RSP))
    {
        /*��ӡ������Ϣ*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtEsmPdnRspMsgTimerWtEsmRspExp: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*��ESM����ID_EMM_ESM_REL_IND*/
    NAS_EMM_EsmSendRelInd();

    /*�޸�״̬��������״̬DEREG��״̬DEREG_PLMN_SEARCH*/
    NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                        EMM_SS_DEREG_NORMAL_SERVICE,
                                        TI_NAS_EMM_STATE_NO_TIMER);
    /*��MMC������Ϣ*/
    /*NAS_EMM_AttSendAppResult(APP_ERR_MM_ATTACH_FAIL_UNSPECIFIED_ABNORMAL);*/
    NAS_EMM_AppSendAttRstDefaultReqType(MMC_LMM_ATT_RSLT_ESM_FAILURE);
    /*ATTACH�����Դ*/
    NAS_EMM_Attach_ClearResourse();

    /* �������CONN̬,�ͷ�����*/
    if (NAS_EMM_CONN_IDLE != NAS_EMM_GetConnState())
    {
        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);
    }

   return   NAS_LMM_MSG_HANDLED;


}
VOS_UINT32    NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgTimerWtEsmBearerCnfExp(VOS_UINT32 ulMsgId,
                                                            const VOS_VOID * pMsgStru)

{
     /*��ӡ����ú���*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgTimerWtEsmBearerCnfExp is entered");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*���״̬�Ƿ�ƥ�䣬����ƥ�䣬�˳�*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_ESM_BEARER_CNF))
    {
        /*��ӡ������Ϣ*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgTimerWtEsmBearerCnfExp: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*֪ͨAPPȱʡ���ؼ���ʧ��*/
    /*NAS_EMM_AttSendAppResult(APP_ERR_MM_ATTACH_FAIL_ACT_DEFAULT_BEARER_FAIL);*/
    NAS_EMM_AppSendAttOtherType(MMC_LMM_ATT_RSLT_ESM_FAILURE);

    /* ����ATTACH������дDETACH���� */
    if (MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI == NAS_EMM_GLO_GetCnAttReqType())
    {
        NAS_EMM_GLO_SetDetTypeMo(MMC_LMM_MO_DET_CS_PS);
    }
    else
    {
        NAS_EMM_GLO_SetDetTypeMo(MMC_LMM_MO_DET_PS_ONLY);
    }

    /*ȥע��*/
    NAS_EMM_SendDetachReqMo();

    return  NAS_LMM_MSG_HANDLED;
}
VOS_VOID  NAS_EMM_ProcAttCompleteSendFail( VOS_VOID )
{
    if (NAS_EMM_GLO_AD_GetAttAtmpCnt()  < 5)
    {
        /*������ʱ��T3411*/
        NAS_LMM_StartPtlTimer(TI_NAS_EMM_PTL_T3411);

        /*�޸�״̬��������״̬DEREG��״̬DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                            TI_NAS_EMM_STATE_NO_TIMER);
        NAS_EMM_AppSendAttFailWithAllPara();
    }

    else
    {
        /* ɾ��GUTI����Զ�����EPS_LOC,������Ҫ������STATUS */
        /*����update statusΪEU2*/
        NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_NOT_UPDATED_EU2;

        /*ɾ��GUTI,KSIasme,TAI list,GUTI*/
        NAS_EMM_ClearRegInfo(NAS_EMM_NOT_DELETE_RPLMN);

        /*������ɾ��TAI List������T3402,˳���ܵߵ�*/
        /*������ʱ��T3402*/
        NAS_LMM_StartPtlTimer(               TI_NAS_EMM_PTL_T3402);

        /* ������ת״̬���ϱ����������� attach attempt counter*/

        /*�޸�״̬��������״̬DEREG��״̬DEREG_ATTEMPTING_TO_ATTACH*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_ATTEMPTING_TO_ATTACH,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        NAS_EMM_AppSendAttFailWithAllPara();
        /*����attach attempt counter*/
        /*NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;*/
    }
    return;
}
VOS_UINT32  NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgSysInfo( VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru  )
{
    EMMC_EMM_SYS_INFO_IND_STRU           *pRcvEmmMsg;

    (VOS_VOID)ulMsgId;

    pRcvEmmMsg                          = (EMMC_EMM_SYS_INFO_IND_STRU *) pMsgStru;

    /*��ӡ����ú���*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgSysInfo is entered");

    /*���״̬�Ƿ�ƥ�䣬����ƥ�䣬�˳�*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_ESM_BEARER_CNF))
    {
        /*��ӡ������Ϣ*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgSysInfo: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*�ж��Ƿ���TA�仯��*/
    if ((EMMC_EMM_CHANGE_PLMN           != pRcvEmmMsg->ulChangeInfo)
        && (EMMC_EMM_CHANGE_TA          != pRcvEmmMsg->ulChangeInfo))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgSysInfo:no change");
        return  NAS_LMM_MSG_HANDLED;
    }

    if ((EMMC_EMM_NO_FORBIDDEN == pRcvEmmMsg->ulForbiddenInfo)
        && (EMMC_EMM_CELL_STATUS_ANYCELL != pRcvEmmMsg->ulCellStatus))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgSysInfo:no forbidden and not any cell");

        /*ֹͣ��ʱ��*/
        NAS_LMM_StopStateTimer(     TI_NAS_EMM_WAIT_ESM_BEARER_CNF);

        /* ����ǽ���ע�ᣬ�����·������ע�� */
        if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
        {
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);

            NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

            /* ����ATTACH CMP��Ϣʧ�ܵĴ��� */
            NAS_EMM_ProcAttCompleteSendFail();

            /* ֪ͨIMSA�ٳ���CS�� */

            NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

            return  NAS_LMM_MSG_HANDLED;
        }

        /*���·�����ͨATTACH*/
        if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        {
            /* ֪ͨESM������EMM��Ҫ������ע�ᣬESM����Ҫ�����̬���еĽ���PDN������¼ */
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
        }
        else
        {
            /*��ESM����ID_EMM_ESM_STATUS_IND��Ϣ*/
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);
        }

        NAS_EMM_EsmSendPdnConnInd(          0);
        NAS_LMM_StartStateTimer(            TI_NAS_EMM_WAIT_ESM_PDN_RSP);
        NAS_EMM_AdStateConvert(             EMM_MS_REG_INIT,
                                            EMM_SS_ATTACH_WAIT_ESM_PDN_RSP,
                                            TI_NAS_EMM_WAIT_ESM_PDN_RSP);

        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_MsRegInitSsWtEsmBearerCnfMsgSysInfo:forbidden or any cell");

    /*ֹͣ��ʱ��*/
    NAS_LMM_StopStateTimer(     TI_NAS_EMM_WAIT_ESM_BEARER_CNF);

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);

        /*״̬Ǩ��:Dereg.Limite_Service*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_LIMITED_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        /*��MMC����LMM_MMC_ATTACH_CNF��LMM_MMC_ATTACH_IND��Ϣ*/
        NAS_EMM_AppSendAttRsltForb(pRcvEmmMsg->ulForbiddenInfo);

        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);

        /* ֪ͨIMSA�ٳ���CS�� */
    }
    else
    {
        if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        {
            NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

            /* ֪ͨESM������EMM��Ҫ������ע�ᣬESM����Ҫ�����̬���еĽ���PDN������¼ */
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
        }
        else
        {
            /*��ESM����ID_EMM_ESM_STATUS_IND��Ϣ*/
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);
        }

        /*״̬Ǩ��:Dereg.Limite_Service*/
        NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                    EMM_SS_DEREG_LIMITED_SERVICE,
                                    TI_NAS_EMM_STATE_NO_TIMER);

        /*��MMC����LMM_MMC_ATTACH_CNF��LMM_MMC_ATTACH_IND��Ϣ*/
        NAS_EMM_AppSendAttRsltForb( pRcvEmmMsg->ulForbiddenInfo);

        NAS_EMM_RelReq(             NAS_LMM_NOT_BARRED);
    }

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRegInitSsWtCnAttCnfMsgSysInfo( VOS_UINT32  ulMsgId,
                                                    VOS_VOID   *pMsgStru)
{
    EMMC_EMM_SYS_INFO_IND_STRU           *pstSysInfoInd = VOS_NULL_PTR;

    (VOS_VOID)ulMsgId;

    pstSysInfoInd                          = (EMMC_EMM_SYS_INFO_IND_STRU *) pMsgStru;

     NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtCnAttCnfMsgSysInfo is entered");

    /*���״̬�Ƿ�ƥ�䣬����ƥ�䣬�˳�*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF))
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtCnAttCnfMsgSysInfo: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }


    /*���ϵͳ��Ϣ���������ͷ����ӣ�����Dereg.Limite_Service*/
    if ((EMMC_EMM_NO_FORBIDDEN != pstSysInfoInd->ulForbiddenInfo)
        ||(EMMC_EMM_CELL_STATUS_ANYCELL == pstSysInfoInd->ulCellStatus))
    {
        /* ��ֹATTACH����*/
        NAS_LMM_StopStateTimer(          TI_NAS_EMM_T3410);

        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtCnAttCnfMsgSysInfo: Cell Forb.");
        if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
        {
            NAS_EMM_StartEmergencyAttach();

            return  NAS_LMM_MSG_HANDLED;
        }

        if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        {
            NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

            /* ֪ͨESM������EMM��Ҫ������ע�ᣬESM����Ҫ�����̬���еĽ���PDN������¼ */
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
        }
        else
        {
            /*��ESM����ID_EMM_ESM_STATUS_IND��Ϣ*/
            NAS_EMM_EsmSendStatResult(EMM_ESM_ATTACH_STATUS_DETACHED);
        }

        /*״̬Ǩ��:Dereg.Limite_Service*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_LIMITED_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);
        /*��MMC����LMM_MMC_ATTACH_CNF��LMM_MMC_ATTACH_IND��Ϣ*/
        NAS_EMM_AppSendAttRsltForb(pstSysInfoInd->ulForbiddenInfo);


        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);

        return  NAS_LMM_MSG_HANDLED;
    }

    /*zhengjunyanע:��������²���ִ������ķ�֧����ʱ����Ŀǰʵ��*/
    /*�ж��Ƿ���TA�仯��*/
    if ((EMMC_EMM_CHANGE_PLMN == pstSysInfoInd->ulChangeInfo)
         || (EMMC_EMM_CHANGE_TA == pstSysInfoInd->ulChangeInfo))
    {
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtCnAttCnfMsgSysInfo: Enter New TA.!");

        /*ֹͣ,������ʱ��T3410*/
        NAS_LMM_StartStateTimer(     TI_NAS_EMM_T3410);

        /*��MRRC����ATTACH REQUEST��Ϣ*/
        NAS_EMM_MrrcSendAttReq();

		/* 33401 CR457������UE������ȥע��̬תȥע��̬ʱ����Ҫ��SIM������NV�еİ�ȫ
		����������Ϊ��Ч*/
        NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_DELETE);

        #if 0
        /* �л�MRRC״̬ΪREADY״̬ */
        NAS_EMM_CHANGE_MRRC_SEND_STATE(     NAS_EMM_MRRC_SEND_STATE_READY);
        #endif
    }

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsDrgSsNmlSrvMsgIntraAttReq( VOS_UINT32  ulMsgId,
                                                  VOS_VOID   *pMsgStru )
{
    NAS_LMM_INTRA_ATTACH_REQ_STRU                     *pRcvEmmMsg;
    pRcvEmmMsg = (NAS_LMM_INTRA_ATTACH_REQ_STRU*)      pMsgStru;

    /*��ӡ����ú���*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsDrgSsNmlSrvMsgIntraAttReq is entered");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    /*���״̬�Ƿ�ƥ�䣬����ƥ�䣬�˳�*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_DEREG,EMM_SS_DEREG_NORMAL_SERVICE))
    {
        /*��ӡ������Ϣ*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsDrgSsNmlSrvMsgIntraAttReq: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*��Ϣ���ݼ��,���д�����ӡ���˳�*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_IntraAttReqChk(pRcvEmmMsg))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_MsDrgSsNmlSrvMsgIntraAttReq: MMC_MM_SYS_INFO_IND_STRU para err!");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;
    }

    /*���津��ATTACH��ԭ��*/
    if(EMM_ATTACH_CAUSE_ESM== NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_GLO_AD_GetAttCau()          = EMM_ATTACH_CAUSE_OTHER;
    }
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /*������UE�����Զ�����ATTACH,�򲻷���ע������*/
    if(NAS_EMM_AUTO_ATTACH_NOT_ALLOW == NAS_EMM_GetPsAttachAllowFlg())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsDrgSsNmlSrvMsgIntraAttReq:AUTO ATTACH NOT ALLOW!");

        /* ��MMC�ϱ�ATTACH��� */
        NAS_EMM_AppSendAttPsNotAllow();

        return NAS_LMM_MSG_HANDLED;
    }
    /*PC REPLAY MODIFY BY LEILI END*/

    /*��ESM����ID_EMM_ESM_PDN_CON_IND��Ϣ*/
    /*NAS_EMM_EsmSendPdnConnInd(          NAS_EMM_GLO_AD_GetDefBeaId());*/
    NAS_EMM_EsmSendPdnConnInd(0);

    /*������ʱ��TI_NAS_EMM_WAIT_ESM_PDN_RSP*/
    NAS_LMM_StartStateTimer(         TI_NAS_EMM_WAIT_ESM_PDN_RSP);

    /*�޸�״̬��������״̬REG_INIT��״̬ATTACH_WAIT_ESM_PDN_RSP*/
    NAS_EMM_AdStateConvert(             EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_ESM_PDN_RSP,
                                        TI_NAS_EMM_WAIT_ESM_PDN_RSP);

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_MsRegSsNmlSrvMsgAuthRej(
                                        VOS_UINT32 ulMsgId,
                                        const VOS_VOID *pMsgStru)
{
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsNmlSrvMsgAuthRej is entered.");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    NAS_EMM_MsRegSsNmlSrvProcMsgAuthRej(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL);

    return NAS_LMM_MSG_HANDLED;
}

/* lihong00150010 emergency tau&service begin */
/*****************************************************************************
 Function Name  : NAS_EMM_MsRegSsLimitedSrvMsgAuthRej
 Discription    : Reg.Limit_Service״̬���յ�AUTH_REJ��Ϣ
 Input          : None
 Output         : None
 Return         :
 History:
      1.lihong00150010      2012-12-29      Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_MsRegSsLimitedSrvMsgAuthRej
(
    VOS_UINT32                          ulMsgId,
    const VOS_VOID                     *pMsgStru
)
{
    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsLimitedSrvMsgAuthRej is entered.");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    NAS_EMM_MsRegSsNmlSrvProcMsgAuthRej(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL);

    return NAS_LMM_MSG_HANDLED;
}
/* lihong00150010 emergency tau&service end */

VOS_UINT32  NAS_EMM_MsRegSsRegAttemptUpdateMmMsgAuthRej
(
    VOS_UINT32 ulMsgId,
    VOS_VOID *pMsgStru
)
{

    NAS_EMM_TAU_LOG_INFO("NAS_EMM_MsRegSsRegAttemptUpdateMmMsgAuthRej is entered.");

    (VOS_VOID)ulMsgId;
    (VOS_VOID)pMsgStru;

    NAS_EMM_MsRegSsNmlSrvProcMsgAuthRej(NAS_EMM_AUTH_REJ_INTRA_CAUSE_NORMAL);

    return NAS_LMM_MSG_HANDLED;
}




VOS_VOID  NAS_EMM_MsRegSsNmlSrvProcMsgAuthRej( VOS_UINT32  ulCause)
{
    MMC_LMM_TAU_RSLT_ENUM_UINT32        ulTauRslt = MMC_LMM_TAU_RSLT_BUTT;
    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_MsRegSsNmlSrvProcMsgAuthRej is entered");

    (VOS_VOID)(                           ulCause);

    /*ֹͣ��ʱ��*/
    NAS_LMM_StopPtlTimer(                TI_NAS_EMM_PTL_T3416);
    NAS_EMM_SecuClearRandRes();

    /*ɾ��GUTI,L.V.R TAI,TAI LIST,KSIasme,����Update Status*/
     /*����update statusΪEU3*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;

    /*ɾ��GUTI,KSIasme,TAI list,GUTI*/
    NAS_EMM_ClearRegInfo(NAS_EMM_DELETE_RPLMN);

    NAS_LMM_SetPsSimValidity(NAS_LMM_SIM_INVALID);

    /*��ESM����ID_EMM_ESM_STATUS_IND��Ϣ*/
    NAS_EMM_EsmSendStatResult(          EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_NO_IMSI,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /*��MMC����LMM_MMC_DETACH_CNF��Ϣ*/
    ulTauRslt = MMC_LMM_TAU_RSLT_AUTH_REJ;
    NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&ulTauRslt);

    /* ����DETACH�ͷ���Դ:��̬�ڴ桢����ֵ */
    NAS_LMM_DeregReleaseResource();

    /*��LRRC����LRRC_LMM_NAS_INFO_CHANGE_REQЯ��USIM��״̬*/
    NAS_EMM_SendUsimStatusToRrc(LRRC_LNAS_USIM_PRESENT_INVALID);

    /*�ȴ�RRC_REL_IND*/
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
    return;
}


VOS_VOID  NAS_EMM_AttResetAttAttempCounter(VOS_VOID)
{
    NAS_EMM_GLO_AD_GetAttAtmpCnt() = 0;
}
VOS_UINT32  NAS_EMM_EmmMsRegInitSsWaitEsmBearerCnfMsgRrcRelInd(
                                                  VOS_UINT32  ulMsgId,
                                                  VOS_VOID   *pMsgStru  )
{
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EmmMsRegInitSsWaitEsmBearerCnfMsgRrcRelInd entered!");
    (VOS_VOID)(                         ulMsgId);
    (VOS_VOID)(                         pMsgStru);

    /*ֹͣ��ʱ��TI_NAS_EMM_WAIT_ESM_BEARER_CNF*/
    NAS_LMM_StopStateTimer(             TI_NAS_EMM_WAIT_ESM_BEARER_CNF);

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_EmmMsRegInitSsWaitEsmBearerCnfMsgRrcRelInd: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttFailWithPara();

        /*����attach attempt counter��˳�������ATTACH����ϱ�֮��*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

        /* ֪ͨESM������EMM��Ҫ������ע�ᣬESM����Ҫ�����̬���еĽ���PDN������¼ */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        NAS_EMM_AdStateConvert(          EMM_MS_DEREG,
                                         EMM_SS_DEREG_PLMN_SEARCH,
                                         TI_NAS_EMM_STATE_NO_TIMER);

        /*send INTRA_CONN2IDLE_REQ����������״̬*/
        NAS_EMM_CommProcConn2Ilde();

        return  NAS_LMM_MSG_HANDLED;
    }

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);

        NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

        /* ����ATTACH CMP��Ϣʧ�ܵĴ��� */
        NAS_EMM_ProcAttCompleteSendFail();

        /*send INTRA_CONN2IDLE_REQ����������״̬*/
        NAS_EMM_CommProcConn2Ilde();

        /* ֪ͨIMSA�ٳ���CS������� */

        return  NAS_LMM_MSG_HANDLED;
    }

    /*��ESM����ID_EMM_ESM_STATUS_IND��Ϣ*/
    NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);

    /*��ESM���������·���ATTACH*/
    /*��ESM����ID_EMM_ESM_PDN_CON_IND��Ϣ*/
    NAS_EMM_EsmSendPdnConnInd(0);

    /*������ʱ��TI_NAS_EMM_WAIT_ESM_PDN_RSP*/
    NAS_LMM_StartStateTimer(         TI_NAS_EMM_WAIT_ESM_PDN_RSP);

    /*�޸�״̬��������״̬REG_INIT��״̬ATTACH_WAIT_ESM_PDN_RSP*/
    NAS_EMM_AdStateConvert(             EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_ESM_PDN_RSP,
                                        TI_NAS_EMM_WAIT_ESM_PDN_RSP);


    /*send INTRA_CONN2IDLE_REQ����������״̬*/
    NAS_EMM_CommProcConn2Ilde();

    return  NAS_LMM_MSG_HANDLED;
}
#if 0
VOS_UINT32  NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgRrcDataCnf(
                                                  VOS_UINT32  ulMsgId,
                                                  VOS_VOID   *pMsgStru  )
{
    LRRC_LMM_DATA_CNF_STRU*               pstRrcMmDataCnf = NAS_EMM_NULL_PTR;

    pstRrcMmDataCnf                     =   (LRRC_LMM_DATA_CNF_STRU*)(pMsgStru);

    NAS_EMM_ATTACH_LOG_NORM(            "NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgRrcDataCnf entered!");
    (VOS_VOID)(                         ulMsgId);
    (VOS_VOID)(                         pMsgStru);

    /* ���OpId */
#if (VOS_OS_VER != VOS_WIN32)

    if(NAS_LMM_FAIL                      ==  NAS_EMM_ChkMmRrcOpId(pstRrcMmDataCnf->ulOpId))
    {
        NAS_EMM_ATTACH_LOG_WARN(        "NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgRrcDataCnf,ChkMmRrcOpId Err");
        return  NAS_LMM_MSG_HANDLED;
    }
#endif

    /*���MRRC�Ĺ�������*/
    NAS_EMM_ClrMrrcMgmtData(            TI_NAS_EMM_WAIT_RRC_DATA_CNF);

    /* ȷ�ϳɹ����ʹ��� */
    if(LRRC_LMM_SEND_RSLT_SUCCESS       == pstRrcMmDataCnf->enSendRslt)
    {
        NAS_EMM_ProcRrcDataCnfSucc();
        return  NAS_LMM_MSG_HANDLED;
    }

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgRrcDataCnf: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttFailWithPara();

        /*����attach attempt counter��˳�������ATTACH����ϱ�֮��*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()  = 0;

        /* ֪ͨESM������EMM��Ҫ������ע�ᣬESM����Ҫ�����̬���еĽ���PDN������¼ */
        NAS_EMM_AttSendEsmResult(       EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                        EMM_SS_DEREG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*����RRC_MM_REL_REQ*/
        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);

        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

    NAS_EMM_ProcAttCompleteSendFail();

    /*����RRC_MM_REL_REQ*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    /* ATTACH����ԭ��ֵ�ڽ������ؼ���ɹ��嶯̬��ʱ��֪ͨEMM��� */
    if (NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
    {
        /* ֪ͨIMSA�ٳ���CS�� */
    }

    return  NAS_LMM_MSG_HANDLED;
}
#endif
VOS_VOID  NAS_EMM_AttNrmFailProc( VOS_VOID )
{
     /*ATTACH���Լ�������1*/
    /*NAS_EMM_GLO_AD_GetAttAtmpCnt()++;*/

    /*�ж�ATTACH���Լ����������ݼ�������С��5�͵���5����*/
    if (NAS_EMM_GLO_AD_GetAttAtmpCnt()  < 5)
    {
        NAS_EMM_AtmpCntLess5Proc();
    }

    else
    {
        NAS_EMM_AtmpCntEqual5Proc();
    }
    return;
}
VOS_UINT32  NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgTmrRrcDataCnfExp(
                                                  VOS_UINT32  ulMsgId,
                                                  VOS_VOID   *pMsgStru  )
{
    NAS_EMM_ATTACH_LOG_NORM(            "NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgTmrRrcDataCnfExp entered!");
    (VOS_VOID)(                         ulMsgId);
    (VOS_VOID)(                         pMsgStru);

    #if 0
    /*���MRRC�Ĺ�������*/
    NAS_EMM_ClrMrrcMgmtData(            TI_NAS_EMM_STATE_NO_TIMER);
    #endif

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgTmrRrcDataCnfExp: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttFailWithPara();

        /*����attach attempt counter��˳�������ATTACH����ϱ�֮��*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()  = 0;

        /* ֪ͨESM������EMM��Ҫ������ע�ᣬESM����Ҫ�����̬���еĽ���PDN������¼ */
        NAS_EMM_AttSendEsmResult(       EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                        EMM_SS_DEREG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*����RRC_MM_REL_REQ*/
        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);

        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

    NAS_EMM_ProcAttCompleteSendFail();

    if(NAS_EMM_CONN_IDLE           != NAS_EMM_GetConnState())
    {
        /*����RRC_MM_REL_REQ*/
        NAS_EMM_RelReq(              NAS_LMM_NOT_BARRED);
    }

    /* ATTACH����ԭ��ֵ�ڽ������ؼ���ɹ��嶯̬��ʱ��֪ͨEMM��� */
    if (NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
    {
        /* ֪ͨIMSA�ٳ���CS�� */
    }

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgSysInfo(
                                                    VOS_UINT32      ulMsgId,
                                                    VOS_VOID        *pMsgStru  )
{
    EMMC_EMM_SYS_INFO_IND_STRU           *pstRcvEmmMsg;

    (VOS_VOID)(ulMsgId);
    pstRcvEmmMsg                        = (EMMC_EMM_SYS_INFO_IND_STRU *) pMsgStru;

    /*��ӡ����ú���*/
    NAS_EMM_ATTACH_LOG_NORM( "NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgSysInfo is entered");

    /*�ж��Ƿ���TA�仯��*/
    if ((EMMC_EMM_CHANGE_PLMN           != pstRcvEmmMsg->ulChangeInfo)
        && (EMMC_EMM_CHANGE_TA          != pstRcvEmmMsg->ulChangeInfo))
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgSysInfo:no change");
        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_RRC_DATA_CNF);
    if((EMMC_EMM_NO_FORBIDDEN         == pstRcvEmmMsg->ulForbiddenInfo)
        && (EMMC_EMM_CELL_STATUS_ANYCELL != pstRcvEmmMsg->ulCellStatus))
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgSysInfo:no forbidden and not any cell");

        #if 0
        /*���MRRC�Ĺ�������*/
        NAS_EMM_ClrMrrcMgmtData(    TI_NAS_EMM_WAIT_RRC_DATA_CNF);
        #endif

        NAS_EMM_SendRrcClearBuffNotify();

        /* ATTACH����ԭ��ֵ�ڽ������ؼ���ɹ��嶯̬��ʱ��֪ͨEMM��� */
        if (NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
        {
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);

            NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

            /* ����ATTACH CMP��Ϣʧ�ܵĴ��� */
            NAS_EMM_ProcAttCompleteSendFail();

            /* ֪ͨIMSA�ٳ���CS�� */

            NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

            return  NAS_LMM_MSG_HANDLED;
        }

        /*������ͨATTACH*/
        if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        {
            /* ֪ͨESM������EMM��Ҫ������ע�ᣬESM����Ҫ�����̬���еĽ���PDN������¼ */
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
        }
        else
        {
            /*��ESM����ID_EMM_ESM_STATUS_IND��Ϣ*/
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);
        }

        NAS_EMM_EsmSendPdnConnInd(      0);
        NAS_LMM_StartStateTimer(        TI_NAS_EMM_WAIT_ESM_PDN_RSP);
        NAS_EMM_AdStateConvert(         EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_ESM_PDN_RSP,
                                        TI_NAS_EMM_WAIT_ESM_PDN_RSP);

        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgSysInfo:forbidden or any cell");

    #if 0
    NAS_EMM_ClrMrrcMgmtData(TI_NAS_EMM_WAIT_RRC_DATA_CNF);
    #endif

    /* ATTACH����ԭ��ֵ�ڽ������ؼ���ɹ��嶯̬��ʱ��֪ͨEMM��� */
    if (NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
    {
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);

        /*״̬Ǩ��:Dereg.Limite_Service*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_LIMITED_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        /*��MMC����LMM_MMC_ATTACH_CNF��LMM_MMC_ATTACH_IND��Ϣ*/
        NAS_EMM_AppSendAttRsltForb(pstRcvEmmMsg->ulForbiddenInfo);

        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);

        /* ֪ͨIMSA�ٳ���CS�� */
    }
    else
    {
        if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
        {
            NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;

            /* ֪ͨESM������EMM��Ҫ������ע�ᣬESM����Ҫ�����̬���еĽ���PDN������¼ */
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
        }
        else
        {
            /*��ESM����ID_EMM_ESM_STATUS_IND��Ϣ*/
            NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);
        }

        /*״̬Ǩ��:Dereg.Limite_Service*/
        NAS_EMM_AdStateConvert(             EMM_MS_DEREG,
                                            EMM_SS_DEREG_LIMITED_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

        /*��MMC����LMM_MMC_ATTACH_CNF��LMM_MMC_ATTACH_IND��Ϣ*/
        NAS_EMM_AppSendAttRsltForb(pstRcvEmmMsg->ulForbiddenInfo);

        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);
    }

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgRrcRelInd(
                                                  VOS_UINT32  ulMsgId,
                                                  VOS_VOID   *pMsgStru  )
{
    (VOS_VOID)(                         pMsgStru);
    (VOS_VOID)(                         ulMsgId);
    NAS_EMM_ATTACH_LOG_NORM(            "NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgRrcRelInd entered!");

    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_RRC_DATA_CNF);

    #if 0
    /*���MRRC�Ĺ�������*/
    NAS_EMM_ClrMrrcMgmtData(            TI_NAS_EMM_STATE_NO_TIMER);
    #endif

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_EmmMsRegInitSsWaitRrcDataCnfMsgRrcRelInd: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttFailWithPara();

        /*����attach attempt counter��˳�������ATTACH����ϱ�֮��*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()  = 0;

        /* ֪ͨESM������EMM��Ҫ������ע�ᣬESM����Ҫ�����̬���еĽ���PDN������¼ */
        NAS_EMM_AttSendEsmResult(       EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                        EMM_SS_DEREG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*send INTRA_CONN2IDLE_REQ����������״̬*/
        NAS_EMM_CommProcConn2Ilde();

        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

    /*send INTRA_CONN2IDLE_REQ����������״̬*/
    NAS_EMM_CommProcConn2Ilde();

    /* ����ATTACH CMP��Ϣʧ�ܵĴ��� */
    NAS_EMM_ProcAttCompleteSendFail();

    /* ATTACH����ԭ��ֵ�ڽ������ؼ���ɹ��嶯̬��ʱ��֪ͨEMM��� */
    if (NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
    {
        /* ֪ͨIMSA�ٳ���CS�� */
    }

    return  NAS_LMM_MSG_HANDLED;

}


/*****************************************************************************
 Function Name   : NAS_EMM_AttGuAttachRst
 Description     : ����״̬�£�����GU������ATTACH���
 Input           :VOS_VOID
 Output          : None
 Return          : VOS_VOID

 History         :
    1.Hanlufeng 41410      2011-5-12  Draft Enact

*****************************************************************************/
VOS_VOID NAS_EMM_AttGuAttachRst( MsgBlock * pMsg )
{
    MMC_LMM_ACTION_RESULT_REQ_STRU     *pMmcActResult;

    pMmcActResult = (MMC_LMM_ACTION_RESULT_REQ_STRU *)pMsg;


    NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_AttGuAttachRst: ulActRst =.",
                            pMmcActResult->ulActRst);

    if(MMC_LMM_RSLT_TYPE_SUCCESS == pMmcActResult->ulActRst)
    {
        NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                        EMM_SS_REG_NO_CELL_AVAILABLE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /* ֪ͨESM״̬�ı� */
        NAS_EMM_PUB_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_ATTACHED);
    }
    else
    {
        NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                        EMM_SS_DEREG_NO_CELL_AVAILABLE,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /* ֪ͨESM״̬�ı� */
        NAS_EMM_PUB_SendEsmStatusInd(EMM_ESM_ATTACH_STATUS_DETACHED);
    }

    return;
}
/*lint +e961*/
/*lint +e960*/

VOS_UINT32 NAS_EMM_SndAttachCmpSuccProc(VOS_VOID* pMsg)
{
    NAS_EMM_ESM_MSG_BUFF_STRU           *pstEsmMsg = NAS_EMM_NULL_PTR;

    (VOS_VOID)pMsg;

    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_SndAttachCmpSuccProc is entered");

    /*���ݴ���ԭ����ESM���ͽ��*/
    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_ATTACHED);

    /*����UEָ���� Drx_Cycle��RRC*/

    if(NAS_EMM_DRX_CYCLE_LEN_CHANGED == NAS_EMM_GetDrxCycleLenChangeFlag())
    {
        NAS_EMM_SendUeSpecDrxCycleLenToRrc();
        NAS_EMM_SetDrxCycleLenChangeFlag(NAS_EMM_DRX_CYCLE_LEN_NOT_CHANGED);
    }

    NAS_MML_SetPsRegContainDrx(NAS_MML_LTE_PS_REG_CONTAIN_DRX_PARA);

    NAS_EMM_GLO_AD_GetAttAtmpCnt() = 0;
    /*leili modify for isr begin*/
    /*���� LMM_MMC_TIN_TYPE_IND*/
    NAS_EMM_SetTinType(MMC_LMM_TIN_GUTI);
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /*�������г��ص�ISR��ʶΪISR����ǰ*/
    NAS_EMM_UpdateBearISRFlag(NAS_EMM_GetPsBearerCtx());
    NAS_EMM_PUBU_LOG_INFO("NAS_EMM_ProcRrcDataCnfSucc: UPDATE MML PS BEARER INFO:");
    NAS_COMM_PrintArray(            NAS_COMM_GET_MM_PRINT_BUF(),
                                    (VOS_UINT8*)NAS_MML_GetPsBearerCtx(),
                                    sizeof(NAS_MML_PS_BEARER_CONTEXT_STRU)
                                    *EMM_ESM_MAX_EPS_BEARER_NUM);
    /*PC REPLAY MODIFY BY LEILI END*/
    /*leili modify for isr end*/
    NAS_EMM_SetAttachRegDomain();

    if ((MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI == NAS_EMM_GLO_GetCnAttReqType()) &&
        (EMM_ATTACH_RST_PS == NAS_EMM_GLO_AD_GetAttRslt()))
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_ProcRrcDataCnfSucc Eps Only");

        NAS_EMM_ProcRrcDataCnfEpsSuccOnly();
    }
    else
    {
        /*�޸�״̬��������״̬REG��״̬REG_NORMAL_SERVICE*/
        if (NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
        {
            NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                            EMM_SS_REG_LIMITED_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

            NAS_LMM_SetEmmInfoRegStatus(NAS_LMM_REG_STATUS_EMC_REGED);
        }
        else
        {
            NAS_EMM_AdStateConvert(         EMM_MS_REG,
                                            EMM_SS_REG_NORMAL_SERVICE,
                                            TI_NAS_EMM_STATE_NO_TIMER);

            NAS_LMM_SetEmmInfoRegStatus(NAS_LMM_REG_STATUS_NORM_REGED);
        }

        pstEsmMsg = (NAS_EMM_ESM_MSG_BUFF_STRU*)NAS_LMM_GetEmmInfoEsmBuffAddr();
        if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
            && (NAS_EMM_NULL_PTR != pstEsmMsg))
        {
            /*��ESM����ID_EMM_ESM_EST_CNF��Ϣ*/
            NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);

            NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_OTHER;
            NAS_EMM_ClearEmcEsmMsg();
        }

        /*ATTACH�����MMC����ATTACH�ɹ�*/
        NAS_EMM_AppSendAttSucc();
    }
    /* ATTACH �ɹ������֮ǰ��������ϵͳ�任����������������ͼ�¼��Ϣ */
    NAS_EMM_ClearResumeInfo();
    /* ATTACH �ɹ������֮ǰ�����ԭ��ֵ */
    NAS_LMM_ClearEmmInfoMmcAttachReason();
    return NAS_EMM_SUCC;
}
VOS_UINT32 NAS_EMM_SndAttachCmpFailProc(VOS_VOID* pMsg,VOS_UINT32 *pulIsDelBuff)
{
    (VOS_VOID)pMsg;
    *pulIsDelBuff = VOS_TRUE;

    NAS_EMM_ATTACH_LOG_INFO("NAS_EMM_SndAttachCmpFailProc is entered");

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        NAS_EMM_ATTACH_LOG_INFO(" NAS_EMM_SndAttachCmpFailProc: CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER");

        NAS_EMM_AppSendAttFailWithPara();

        /*����attach attempt counter��˳�������ATTACH����ϱ�֮��*/
        NAS_EMM_GLO_AD_GetAttAtmpCnt()  = 0;

        /* ֪ͨESM������EMM��Ҫ������ע�ᣬESM����Ҫ�����̬���еĽ���PDN������¼ */
        NAS_EMM_AttSendEsmResult(       EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);

        NAS_EMM_AdStateConvert(         EMM_MS_DEREG,
                                        EMM_SS_DEREG_PLMN_SEARCH,
                                        TI_NAS_EMM_STATE_NO_TIMER);

        /*����RRC_MM_REL_REQ*/
        NAS_EMM_RelReq(                 NAS_LMM_NOT_BARRED);

        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_EMM_AttSendEsmResult(   EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_GLO_AD_GetAttAtmpCnt()++;

    NAS_EMM_ProcAttCompleteSendFail();

    /*����RRC_MM_REL_REQ*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    /* ATTACH����ԭ��ֵ�ڽ������ؼ���ɹ��嶯̬��ʱ��֪ͨEMM��� */
    if (NAS_LMM_REG_STATUS_EMC_REGING == NAS_LMM_GetEmmInfoRegStatus())
    {
        /* ֪ͨIMSA�ٳ���CS�� */
    }

    return NAS_EMM_SUCC;
}




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
