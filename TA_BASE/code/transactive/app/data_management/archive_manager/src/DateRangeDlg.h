/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/DateRangeDlg.h $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Dialog for operator to select the range to be manually archived.
  */

#if !defined(AFX_DATERANGEDLG_H__9A690582_B4C0_45F2_A535_845BF92B1782__INCLUDED_)
#define AFX_DATERANGEDLG_H__9A690582_B4C0_45F2_A535_845BF92B1782__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <time.h>
#include <afx.h>

#include "app/data_management/archive_manager/src/resource.h"

namespace TA_Base_App
{
    class DateRangeDlg : public CDialog
    {
    public:
        /** 
          * DateRangeDlg
          *
          * Standard constructor.
          *
          * @param      CWnd* pParent
          *             The parent window.
          */

        DateRangeDlg(CWnd* pParent = NULL);

        /**
          * ~DateRangeDlg
          *
          * Standard destructor.
          */

        virtual ~DateRangeDlg() {};

        /**
          * getSelectedFromDate
          *
          * Gets the date that has been selected in the FromDate date picker.
          *
          * @return     time_t
          *             The FromDate.
          */
        
        time_t getSelectedFromDate() { return m_fromDate; };

        /**
          * getSelectedToDate
          *
          * Gets the date that has been selected in the ToDate date picker.
          *
          * @return     time_t
          *             The ToDate.
          */
        
        time_t getSelectedToDate() { return m_toDate; };

       // KT 25 Mar 04: Use COleDateTime instead of CTime - this goes for longer (up to 9999).
       
        // Dialog Data
        //{{AFX_DATA(DateRangeDlg)
	    enum { IDD = IDD_DATE_RANGE_DIALOG };
	    COleDateTime	m_fromDatePickerValue;
	    COleDateTime	m_toDatePickerValue;
		UINT			m_dvdVolume;
	    //}}AFX_DATA
  
		// yangguang++ TD16713
		void setDvdVolume(int newVolume)  { m_dvdVolume =  newVolume; };
		// ++yangguang TD16713		
        
        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(DateRangeDlg)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL
        
        // Implementation
    protected:

        // Generated message map functions
        //{{AFX_MSG(DateRangeDlg)
        virtual BOOL OnInitDialog();
        virtual void OnOK();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        //
        // Disable the copy constructor and assignment operator.
        //

        DateRangeDlg( const DateRangeDlg& );
        DateRangeDlg& operator=( const DateRangeDlg& );

        //
        // These dates will be filled with the start and end dates for
        // which data is to be retrieved (manually archived).
        //

        time_t m_fromDate;
        time_t m_toDate;
    };
    
} // TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.  

#endif // !defined(AFX_DATERANGEDLG_H__9A690582_B4C0_45F2_A535_845BF92B1782__INCLUDED_)
