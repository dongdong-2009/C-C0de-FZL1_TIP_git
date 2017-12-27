/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_manager/src/ExportAlarms.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// ExportAlarms.h: interface for the CExportAlarms class.

//

//////////////////////////////////////////////////////////////////////



#if !defined(AFX_EXPORTALARMS_H__E1B3DDE2_9D53_11D1_8BAC_0000B43382FE__INCLUDED_)

#define AFX_EXPORTALARMS_H__E1B3DDE2_9D53_11D1_8BAC_0000B43382FE__INCLUDED_



#if _MSC_VER >= 1000

#pragma once

#endif // _MSC_VER >= 1000



#include <string>
#include "bus\alarm\alarm_list_control\src\AlarmListCtrl.h"



class CExportAlarms : public CWinThread

{

		DECLARE_DYNCREATE(CExportAlarms)



public:

	CExportAlarms() : m_alarmList(NULL) { };

	virtual ~CExportAlarms(){};



	// Since this is a thread these are parameters to receive from the calling class.

	struct t_ParamsToReceive

	{

		CListCtrl *alarmList;
        CString defaultExportPath;
		std::vector<CString>* pImageStringVector;
	};



// Overrides

	// ClassWizard generated virtual function overrides

	//{{AFX_VIRTUAL(CExportAlarms)

	public:

	virtual BOOL InitInstance();

	virtual int ExitInstance();

	//}}AFX_VIRTUAL



protected:

	// This is called when the START_EXPORT message is received. This kicks off

	// an export/save.

	afx_msg void saveNow(WPARAM wParam, LPARAM lParam);



	// Generated message map functions

	//{{AFX_MSG(CExportAlarms)

		// NOTE - the ClassWizard will add and remove member functions here.

	//}}AFX_MSG



	DECLARE_MESSAGE_MAP()



private:



	// This method does the actual save.

	void OnSave();



	// Converts text into a safe format to be output in a comma separated file.

	// eg it adds quotes around the text if there are commas

	void convertText(std::string& text);



	CListCtrl* m_alarmList;

    CString m_defaultExportPath;

	std::vector<CString>* m_imageStringVector;

	static const std::string EXPORTALARMS_LOCALPATH;
};



#endif // !defined(AFX_EXPORTALARMS_H__E1B3DDE2_9D53_11D1_8BAC_0000B43382FE__INCLUDED_)

