#if !defined(AFX_REGIONMANAGER_H__71E7EFBC_6857_4284_8B43_039F09E3A7C1__INCLUDED_)
#define AFX_REGIONMANAGER_H__71E7EFBC_6857_4284_8B43_039F09E3A7C1__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/RegionManager.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Consolidates all the required includes in a single header.
  *
  */

#ifdef __WIN32__
	#pragma warning(disable: 4250 4786)
#endif

// System includes
#include <sstream>
#include <string>
#include <vector>

// Core includes
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/data_access_interface/entity_access/src/RegionManagerEntityData.h"

// Bus includes
#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/AbstractDialogGUI.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/generic_gui/src/TransActiveWinApp.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/user_settings/src/SettingsMgr.h"
#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"
#include "bus/mfc_extensions/src/mfc_extensions.h"
#include "bus\security\duty_monitor_framework\src\DutyDataStore.h"

// Region Manager includes
#include "app/security/region_manager/src/Resource.h"
#include "app/security/region_manager/src/MessageBox.h"
#include "app/security/region_manager/src/GridAssistant.h"
#include "app/security/region_manager/src/AllOperatorsPropertyPage.h"
#include "app/security/region_manager/src/ActiveOperatorsPropertyPage.h"
#include "app/security/region_manager/src/SubsystemsPropertyPage.h"
#include "app/security/region_manager/src/RegionManagerPropertySheet.h"
#include "app/security/region_manager/src/RegionDutyTransferDlg.h"
#include "app/security/region_manager/src/RegionDutyRemovalDlg.h"
#include "app/security/region_manager/src/RegionDutyChangeConfirmDlg.h"
#include "app/security/region_manager/src/RegionManagerDlg.h"
#include "app/security/region_manager/src/RegionManagerGUI.h"
#include "app/security/region_manager/src/RegionManagerApp.h"

#endif // !defined(AFX_REGIONMANAGER_H__71E7EFBC_6857_4284_8B43_039F09E3A7C1__INCLUDED_)