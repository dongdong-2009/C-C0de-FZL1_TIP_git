#if !defined(AFX_REGIONMANAGERGUI_H__831E441D_66B6_4B53_946E_38E9A4F36A31__INCLUDED_)
#define AFX_REGIONMANAGERGUI_H__831E441D_66B6_4B53_946E_38E9A4F36A31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/RegionManagerGUI.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class provides an interface between GenericGUI and Region Manager. 
  * GenericGUI uses this class the app to various system changes and requests.
  *
  */

namespace TA_Base_App
{
    class RegionManagerGUI : public TA_Base_Bus::AbstractDialogGUI
    {
    // Operations
    public:
        RegionManagerGUI();
        ~RegionManagerGUI();
		//TD18095, jianghp, to fix the performance of showing manager application
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
    };

} // namespace TA_Base_App

#endif // !defined(AFX_REGIONMANAGERGUI_H__831E441D_66B6_4B53_946E_38E9A4F36A31__INCLUDED_)
