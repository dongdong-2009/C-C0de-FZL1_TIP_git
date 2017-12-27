/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * <description>
  *
  **/

#if !defined(AFX_PLANSTEPEDITBackgroundLaunchSchematicPARAMETERPAGE_H__3F3A7438_8E80_4538_8BAF_685BED09AEF4__INCLUDED_)
#define AFX_PLANSTEPEDITBackgroundLaunchSchematicPARAMETERPAGE_H__3F3A7438_8E80_4538_8BAF_685BED09AEF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC)

#include "NumericStepParameter.h"
#include "DataPointStepParameter.h"
#include "PlanStepEditParameterPage.h"


namespace TA_Base_App
{
    class BackgroundLaunchSchematicStep;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditBackgroundLaunchSchematicParameterPage dialog

    class CPlanStepEditBackgroundLaunchSchematicParameterPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditBackgroundLaunchSchematicParameterPage)

    // Construction/destruction
    public:
        CPlanStepEditBackgroundLaunchSchematicParameterPage(BackgroundLaunchSchematicStep* step);
        ~CPlanStepEditBackgroundLaunchSchematicParameterPage();


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditBackgroundLaunchSchematicParameterPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_BACKGROUND_LAUNCH_SCHEMATIC_DLG };
        UINT    m_screenNumber;
        CSpinButtonCtrl    m_screenNumberSpin;
        CComboBox    m_schematicNameCombo;
        CString    m_commandLine;
         CEdit   m_commandLineTxt;
        CComboBox    m_actionTypeCombo;
        CString    m_dataPointLabel;
        CComboBox    m_operatorProfileCombo;

        DataPointStepParameter m_dataPointParameter;
        NumericStepParameter m_actionTypeKey;
        NumericStepParameter m_operatorProfileParameter;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditBackgroundLaunchSchematicParameterPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditBackgroundLaunchSchematicParameterPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnPickDataPointButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // defined (STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC)

#endif // !defined(AFX_PLANSTEPEDITBackgroundLaunchSchematicPARAMETERPAGE_H__3F3A7438_8E80_4538_8BAF_685BED09AEF4__INCLUDED_)
