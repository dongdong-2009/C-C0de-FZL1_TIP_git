#if !defined(AFX_DUTYMANAGERGUI_H__3A5AFA78_E277_4FE9_82FE_888D7BE84202__INCLUDED_)
#define AFX_DUTYMANAGERGUI_H__3A5AFA78_E277_4FE9_82FE_888D7BE84202__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/src/DutyManagerGUI.h $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/07/13 15:30:21 $
  * Last modified by:  $Author: liwei.gong $
  *
  * This class provides an interface between GenericGUI and Duty Manager. 
  * GenericGUI uses this class the app to various system changes and requests.
  *
  */

namespace TA_Base_App
{
    class DutyManagerGUI : public TA_Base_Bus::AbstractDialogGUI
    {
    // Operations
    public:
        DutyManagerGUI();
        ~DutyManagerGUI();

		virtual void onInitGenericGUICompleted();
        /**
          * AbstractGUIApplication interface
          *
          */
        virtual void createApplicationObject();
        virtual void prepareForClose();

        /**
          * IGUIApplication interface
          *
          */
        virtual unsigned long getApplicationType();
        virtual void checkEntity(TA_Base_Core::IEntityData* guiEntity);
        virtual void checkCommandLine();
        virtual void entityChanged(const std::vector<std::string>& changes);
        virtual void serverIsDown();
        virtual void serverIsUp();

        /**
          * IGUIAccess interface
          *
          */
        virtual void loadAllUserSettings();
        virtual void saveAllUserSettings();
	protected:
		virtual void terminateGUI();
	private:
		 static BOOL CALLBACK MyWndEnumHandler(HWND hWnd, LPARAM lParam);
    };

} // namespace TA_Base_App

#endif // !defined(AFX_DUTYMANAGERGUI_H__3A5AFA78_E277_4FE9_82FE_888D7BE84202__INCLUDED_)
