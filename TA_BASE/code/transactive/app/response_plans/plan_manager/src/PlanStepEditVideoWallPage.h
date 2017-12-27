//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditVideoWallPage.h $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// This class handles the dialog interaction for aswallboarding a layout to a wallboard on the VideoWall.


#if !defined(AFX_PLANSTEPEDITVIDEO_WALLPAGE_H__1F5AED02_B755_42B9_BE63_F95399165498__INCLUDED_)
#define AFX_PLANSTEPEDITVIDEO_WALLPAGE_H__1F5AED02_B755_42B9_BE63_F95399165498__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_VIDEOWALL)

#include <map>

#include "PlanStepEditParameterPage.h"
#include "WallboardStepParameter.h"
#include "WallboardLayoutStepParameter.h"
#include "core/data_access_interface/entity_access/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_App
{
    class VideoWallStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditVideoWallPage dialog

    class CPlanStepEditVideoWallPage : public CPlanStepEditParameterPage
    {
           DECLARE_DYNAMIC(CPlanStepEditVideoWallPage)

    // Types
    private:
        typedef std::map< std::string, unsigned long > NameToKeyMap;
        typedef std::map< unsigned long, std::string > KeyToNameMap;


    // Construction
    public:
        CPlanStepEditVideoWallPage(VideoWallStep* step);
        ~CPlanStepEditVideoWallPage() {};


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

    private:
        /**
          * PopulateWallboardCombo
          *
          * Populates the combo box with all the wallboards.
          */
        void PopulateWallboardCombo();

        /**
          * PopulateLayoutCombo
          *
          * Populates the layout combo box with all the
          * layouts for the currently selected wallboard.
          * This will display all required error message boxes to the user.
          *
          * @param      unsigned long wallboardKey
          *             The key of the wallboard to load the layouts for.
          */
        void PopulateLayoutCombo( unsigned long wallboardKey );

        /**
          * GetLayoutsForWallboard
          *
          * Retrieves the layouts for the currently selected wallboard
          * from the VideoWallLayoutAgent.
          *
          * @return     std::map< std::string, unsigned long >
          *             A map of the layout names to ids.
          */
        NameToKeyMap GetLayoutsForWallboard();

        /**
          * GetWallboardDetails
          *
          * Retrieves the details for all the wallboards monitored by the
          * VideoSwitchAgent.
          *
          * @return     std::map< std::string, unsigned long >
          *             A map of the wallboard names to entity keys.
          */
        NameToKeyMap GetWallboardDetails();


    // Attributes
    private:
        // 
        // Map of wallboard key to the agent controlling that wallboard.
        // 
        KeyToNameMap m_agents;

        // 
        // The parameters for this step.
        // 
           WallboardStepParameter m_wallboardParameter;
        WallboardLayoutStepParameter m_layoutParameter;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditVideoWallPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_VIDEO_WALL_DLG };
        CComboBox    m_wallboards;
        CComboBox    m_layouts;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditVideoWallPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditVideoWallPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnSelectionChangeWallboardList();
        afx_msg void OnSelectionChangeLayoutList();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // defined (STEPTYPE_VIDEOWALL)

#endif // !defined(AFX_PLANSTEPEDITVIDEO_WALLPAGE_H__1F5AED02_B755_42B9_BE63_F95399165498__INCLUDED_)
