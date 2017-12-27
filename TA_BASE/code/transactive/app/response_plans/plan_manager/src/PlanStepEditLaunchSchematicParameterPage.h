/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditLaunchSchematicParameterPage.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#if !defined(AFX_PLANSTEPEDITLAUNCHSCHEMATICPARAMETERPAGE_H__E758CC5E_8F9A_4417_9A28_32AD27FE7798__INCLUDED_)
#define AFX_PLANSTEPEDITLAUNCHSCHEMATICPARAMETERPAGE_H__E758CC5E_8F9A_4417_9A28_32AD27FE7798__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_LAUNCH_SCHEMATIC)

#include "PlanStepEditParameterPage.h"


namespace TA_Base_App
{
    class LaunchSchematicStep;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditLaunchSchematicParameterPage dialog

    class CPlanStepEditLaunchSchematicParameterPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditLaunchSchematicParameterPage)

    // Construction/destruction
    public:
        CPlanStepEditLaunchSchematicParameterPage(LaunchSchematicStep* step);
        ~CPlanStepEditLaunchSchematicParameterPage();


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditLaunchSchematicParameterPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_LAUNCH_SCHEMATIC_DLG };
        UINT    m_screenNumber;
        CSpinButtonCtrl    m_screenNumberSpin;
        CComboBox    m_schematicNameCombo;
        CString    m_commandLine;
         CEdit   m_commandLineTxt;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditLaunchSchematicParameterPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditLaunchSchematicParameterPage)
        virtual BOOL OnInitDialog();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // defined (STEPTYPE_LAUNCH_SCHEMATIC)

#endif // !defined(AFX_PLANSTEPEDITLAUNCHSCHEMATICPARAMETERPAGE_H__E758CC5E_8F9A_4417_9A28_32AD27FE7798__INCLUDED_)
