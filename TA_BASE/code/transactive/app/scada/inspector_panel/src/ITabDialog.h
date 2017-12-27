/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/ITabDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifndef ITABDIALOG_H
#define ITABDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/scada/inspector_panel/src/resource.h"
#include "bus/scada/common_library/src/CommonDefs.h"


namespace TA_Base_Core
{
	class RightsLibrary;
}

namespace TA_Base_App
{
	class CEquipment;

	class ITabDialog
	{
	// Construction
	public:
		virtual void setEquipment(CEquipment* equipment) = 0;
		virtual void updateDataPoint(unsigned long key, TA_Base_Bus::ScadaEntityUpdateType updateType) = 0;
		virtual void updateDataNode(TA_Base_Bus::ScadaEntityUpdateType updateType) = 0;
		virtual bool isUnsavedChangeOnTab() = 0;
		virtual bool isOkToChangeTab() = 0;
		virtual bool isAnyDataPointsDisplayedInTab() const = 0;
		virtual void getWindowRect(CRect* rect) = 0;
		virtual unsigned int getMinimumHeight() = 0;
		virtual void resize(unsigned int height) = 0;
		virtual void updatePermissions(/*TA_Base_Bus::RightsLibrary* rightsMgr*/) = 0;
		virtual void resetList() = 0;
		virtual void displayDataPoints()=0;

		ITabDialog() {};
		virtual ~ITabDialog() {};
	};
}

#endif // #ifndef ITABDIALOG_H
