/**
* The source code in this file is the property of 
* ST Electronics and is not for redistribution
* in any form.
*
* Source:    
* @author:  Noel R. Tajanlangit
* @version: $Revision: #0.9.0.0 $
*
* This file provides the interface definition for C952-C955 Alarm interface 
* specific to show related schematic from C952 library to C955 bridge module.
*  
*/

#include "ISCSAlarmActionBridge.h"
#include "ISCSAlarmActionBridgeEventHandler.h"
#include "NonReEntrantThreadLockable.h"

using namespace ISCSAlarmActionBridge;

#pragma once

class CISCSAlarmActionBridge_impl
{
public:
	static CISCSAlarmActionBridge_impl* getInstance();
	static void releaseInstance();

	/**
	* showSchematic
	*
	* This function calls to show schematic. 
	**
	* @param	SchematicDisplayInfo & schematic display information
	*
	*/   
	ShowSchematicResult showSchematic(const SchematicDisplayInfo & schematicInfo);
  

	void registerCallBack(ShowSchematicResult (*eventshematicCallback)(const SchematicDisplayInfo & schematicInfo) );

	void unRegisterCallBack(void);

private:
	CISCSAlarmActionBridge_impl(void);
	~CISCSAlarmActionBridge_impl(void);

	static TA_Base_Bus::NonReEntrantThreadLockable m_singletonLock;
	static CISCSAlarmActionBridge_impl* m_pInstance;
	TA_Base_Bus::NonReEntrantThreadLockable m_threadLock;
};
