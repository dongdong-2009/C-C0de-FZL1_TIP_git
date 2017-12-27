#if !defined(AFX_SUBSYSTEMSPROPERTYPAGE_H__364ED1F0_9217_495F_BA1D_AD786D4C4594__INCLUDED_)
#define AFX_SUBSYSTEMSPROPERTYPAGE_H__364ED1F0_9217_495F_BA1D_AD786D4C4594__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/SubsystemsPropertyPage.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class represents the "Subsystems" tab. It displays the active 
  * subsystem duty state for every physical subsystem in every region.
  *
  */

namespace TA_Base_App
{
    class SubsystemsPropertyPage : public CPropertyPage,
                                   public TA_Base_Bus::ISubsystemDutyObserver
    {
	    DECLARE_DYNCREATE(SubsystemsPropertyPage)

    // Operations
    public:
	    SubsystemsPropertyPage();
	    virtual ~SubsystemsPropertyPage();

        // ISubsystemDutyObserver interface
        virtual void dutyChanged(const TA_Base_Bus::SubsystemDutyUpdate& dutyUpdate);

    protected:
        void setUncontrolledRegionIndicator(int column);


    // Overrides
	    // ClassWizard generate virtual function overrides
	    //{{AFX_VIRTUAL(SubsystemsPropertyPage)
	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL


    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(SubsystemsPropertyPage)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg LRESULT OnReceiveSubsystemDutyUpdate(WPARAM wParam, LPARAM lParam);
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    // Dialog Data
    private:
	    //{{AFX_DATA(SubsystemsPropertyPage)
	    enum { IDD = IDD_SUBSYSTEMS_DLG };
		    // NOTE - ClassWizard will add data members here.
		    //    DO NOT EDIT what you see in these blocks of generated code !
	    //}}AFX_DATA

        CGridCtrl m_subsystemDutyGrid;

        TA_Base_Bus::DataCache& m_dataCache;
        TA_Base_Bus::SubsystemDutyMatrix& m_subsystemDutyMatrix;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_SUBSYSTEMSPROPERTYPAGE_H__364ED1F0_9217_495F_BA1D_AD786D4C4594__INCLUDED_)
