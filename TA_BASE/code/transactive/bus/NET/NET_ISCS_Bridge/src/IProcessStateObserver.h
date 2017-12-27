/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: $
  * @author 
  * @version $Revision:  $
  * Last modification: $DateTime: $
  * Last modified by:  $Author:  $
  * 
  * This Interface performs Process state related function.  
  */

#pragma once

using namespace System;
#include "bus/generic_gui/src/IGUIApplication.h"

namespace NET_ISCS_Bridge 
{

	public interface class IProcessStateObserver
	{
		void CheckCommandLine();
		int DutyChanged();
		void EntityChanged();
		void OperationModeChanged();
		void PreTerminateApplication(TA_Base_Core::EManagedProcessTerminateCode TerminateState);
		void ServerStateChange(bool ServerState);
		void TerminateApplication();
		long GetApplicationtype();
		void setGuiEntity(TA_Base_Core::IEntityData* guiEntity);
		void onInitGenericGUICompleted();
	};


}