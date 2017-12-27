/**
* The source code in this file is the property of 
* ST Electronics and is not for redistribution
* in any form.
*
* Source:    
* @author:  Noel R. Tajanlangit
* @version: $Revision: #1.0.0.0 $
*
* This file provides the interface definition for C955-C952 Alarm interface 
* specific to open related schematic from ISCS alarm manager and alarm banner.
*  
*/

#ifndef __ISCSALARMAACTIONBRIDGEEVENTHANDLER_H__
#define __ISCSALARMAACTIONBRIDGEEVENTHANDLER_H__

#include "ISCSAlarmActionBridge.h"

#pragma  once;

namespace ISCSAlarmActionBridge
{
	ISCSALARMACTIONBRIDGE_API void registerCallBack(ShowSchematicResult (*eventshematicCallback)(const SchematicDisplayInfo & schematicInfo) );
	
	ISCSALARMACTIONBRIDGE_API void unRegisterCallBack(void);
}

#endif // __ISCSALARMAACTIONBRIDGEEVENTHANDLER_H__