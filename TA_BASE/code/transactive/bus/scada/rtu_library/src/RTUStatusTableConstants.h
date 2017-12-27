#ifndef RTUSTATUSTABLECONSTANTS_H
#define RTUSTATUSTABLECONSTANTS_H

/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/RTUStatusTableConstants.h $
  * @author  hoav
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/21 16:51:05 $
  * Last modified by : $Author: haijun.li $
  *
  *
  *	This files contains the external subsystem IDs that have
  * been assigned to the word 13 to 31 of the RTU Status Table.
  *
  * NOTE: This file is to be modified to meet the project requirement.
  *
  */


//
// External SWC System ID associated with words 13 to 32 of the FEP Status Table
//

typedef enum
{
	WORD13_RTU_STATUS_TABLE = 33,	// AFC
	WORD14_RTU_STATUS_TABLE = 49,	// ACS
	WORD15_RTU_STATUS_TABLE = 65,	// PIDS
	WORD16_RTU_STATUS_TABLE = -9,	// UNDEFINED
	WORD17_RTU_STATUS_TABLE = 97,	// PSD
	WORD18_RTU_STATUS_TABLE = 113,	// FAS
	WORD19_RTU_STATUS_TABLE = 129,	// BAS
	WORD20_RTU_STATUS_TABLE = 145,	// CCTV
	WORD21_RTU_STATUS_TABLE = 161,	// FG
	WORD22_RTU_STATUS_TABLE = 177,	// PA
	WORD23_RTU_STATUS_TABLE = 193,	// TIS
	WORD24_RTU_STATUS_TABLE = 80,	// CLK
	WORD25_RTU_STATUS_TABLE = -1,	// UNDEFINED
	WORD26_RTU_STATUS_TABLE = -2,	// UNDEFINED
	WORD27_RTU_STATUS_TABLE = -3,	// UNDEFINED
	WORD28_RTU_STATUS_TABLE = -4,	// UNDEFINED
	WORD29_RTU_STATUS_TABLE = -5,	// UNDEFINED
	WORD30_RTU_STATUS_TABLE = -6,	// UNDEFINED
	WORD31_RTU_STATUS_TABLE = -7,	// UNDEFINED
	WORD32_RTU_STATUS_TABLE = -8	// UNDEFINED
} SWCSystemID;

#endif
