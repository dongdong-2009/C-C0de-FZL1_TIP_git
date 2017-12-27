// ISCSAlarmActionBridge.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ISCSAlarmActionBridge.h"
#include "ISCSAlarmActionBridgeEventHandler.h"
#include "ISCSAlarmActionBridge_impl.h"

namespace ISCSAlarmActionBridge
{

	ISCSALARMACTIONBRIDGE_API ShowSchematicResult  showSchematic(const SchematicDisplayInfo& schematicInfo)
	{
		return CISCSAlarmActionBridge_impl::getInstance()->showSchematic(schematicInfo);
	}

	ISCSALARMACTIONBRIDGE_API void registerCallBack(ShowSchematicResult (*eventshematicCallback)(const SchematicDisplayInfo & schematicInfo) )
	{
		CISCSAlarmActionBridge_impl::getInstance()->registerCallBack(eventshematicCallback);
	}

	ISCSALARMACTIONBRIDGE_API void unRegisterCallBack(void)
	{
		CISCSAlarmActionBridge_impl::getInstance()->unRegisterCallBack();
	}
}

