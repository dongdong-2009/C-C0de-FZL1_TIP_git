//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanManagerGUI.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANMANAGERGUI_H__3D9F35A5_5E2D_47E9_872E_A3631C412853__INCLUDED_)
#define AFX_PLANMANAGERGUI_H__3D9F35A5_5E2D_47E9_872E_A3631C412853__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4786)
#include "bus/generic_gui/src/AbstractDocumentGUI.h"
#include "core/utilities/src/RunParams.h"
#include <vector>

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PlanManagerGUI
    class CPlanBrowserDoc;

    class PlanManagerGUI : public TA_Base_Bus::AbstractDocumentGUI,
                           public TA_Base_Core::RunParamUser
    {
    // Operations
    public:
        PlanManagerGUI();
        ~PlanManagerGUI();

        // TD18095, jianghp
        virtual void onInitGenericGUICompleted();

        /////////////////////////////////////////////////////////////////////////////
        // AbstractGUIApplication operations
        virtual void createApplicationObject();
        // TD 11846 Fix: Changed setPosition's signature to follow AbstractGUIApplication's setPosition's signature.
        virtual void setPosition(const unsigned long posFlag, const unsigned long alignFlag, const RECT& objectDim, const RECT& boundaryDim );
        virtual void prepareForClose();

        /////////////////////////////////////////////////////////////////////////////
        // IGUIApplication operations
        virtual unsigned long getApplicationType();
        virtual void checkEntity(TA_Base_Core::IEntityData* guiEntity);
        virtual void checkCommandLine();
        virtual void entityChanged(const std::vector<std::string>& changes);
        virtual void serverIsDown();
        virtual void serverIsUp();

        /////////////////////////////////////////////////////////////////////////////
        // IGUIAccess operations
        virtual void loadAllUserSettings();
        virtual void saveAllUserSettings();

        /////////////////////////////////////////////////////////////////////////////
        // RunParamUser interface
        virtual void onRunParamChange(const std::string& name, const std::string& value);

    protected:
        void selectStartupView();

    // Attributes
    private:
        bool m_execPlanIdChanged;
        bool m_viewPlanIdChanged;
        bool m_assocAlarmIdChanged;
        typedef std::vector<CPlanBrowserDoc *> DocumentList;
        DocumentList m_vecDocList;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANMANAGERGUI_H__3D9F35A5_5E2D_47E9_872E_A3631C412853__INCLUDED_)
