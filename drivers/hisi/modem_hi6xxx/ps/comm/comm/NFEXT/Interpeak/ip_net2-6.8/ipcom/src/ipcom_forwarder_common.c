/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_forwarder_common.c,v $ $Revision: 1.2.8.2 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_forwarder_common.c,v $
 *     $Author: ulf $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 *
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#define IPCOM_FORWARDER_DATA_C

#include "ipcom_config.h"

#ifdef IPCOM_USE_FORWARDER

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

/* Ipcom include files */
#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_err.h"
#include "ipcom_os.h"

#include <ipcom_cstyle.h>
#include <ipcom_netif.h>
#include <ipcom_pkt.h>
#include <ipcom_syslog.h>
#include <ipcom_type.h>
#include <ipcom_pipe.h>
#include <ipcom_shell.h>
#include <ipcom_getopt.h>
#include <ipcom_atomic.h>


#include <ipnet.h>
#include <ipnet_config.h>
#include <ipnet_netif.h>
#include <ipnet_eth.h>
#include <ipnet_route.h>
#include "ipnet_usr_sock.h"

#include "ipcom_forwarder.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

Ipcom_fw_t ipcom_fw = {0}; /* Duplicated between VBs. */

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#else
int ipcom_forwarder_common_empty_file;
#endif /* IPCOM_USE_FORWARDER */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
