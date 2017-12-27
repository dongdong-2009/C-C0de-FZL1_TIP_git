/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/tab_control_with_disable/TabCtrlWithDisableEx.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * CTabCtrlWithDisableEx manages a tab control that allows the client code
  *     to disable specific tab indexes using the function EnableTabIndex
  * (extension of CTabCtrlWithDisable)
  *
  */


#pragma once

#include <map>

#include "bus/mfc_extensions/src/tab_control_with_disable/TabCtrlWithDisable.h"

namespace TA_Base_Bus
{
    #ifdef IMPORT_EXTENSIONS
    class __declspec(dllimport) CTabCtrlWithDisableEx : public CTabCtrlWithDisable
    #else
    class AFX_EXT_CLASS CTabCtrlWithDisableEx : public CTabCtrlWithDisable
    #endif
    {
    public:
    
        /**
         * IsTabEnabled (interface implementation)
         *
         * @see CTabCtrlWithDisable::IsTabEnabled for more details)
         *
         */
        virtual	BOOL IsTabEnabled(int iTab);
    
        /**
         * EnableTabIndex
         *
         * Updates the enables state of the tab at the specified index
         *
         * @param iTab the index of the tab
         *
         * @param enable true to enable the tab, false to have it
         *          this class return return false for IsTabEnabled(iTab)
         *
         */
        void EnableTabIndex(int iTab, bool enable);
    
    private:
    
        // Map of enabled states, by default a tab is disabled if there
        // is no entry for it in the map
        std::map<int, bool> m_enabledMap;
    };

} // namespace TA_Base_Bus
