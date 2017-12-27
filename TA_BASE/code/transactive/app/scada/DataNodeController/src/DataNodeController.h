/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/DataNodeController/src/DataNodeController.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_DATANODECONTROLLER_H__1ACF5E6B_3440_4FFC_8DAE_034BF0EC2F77__INCLUDED_)
#define AFX_DATANODECONTROLLER_H__1ACF5E6B_3440_4FFC_8DAE_034BF0EC2F77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "bus/generic_gui/src/TransActiveWinApp.h"


namespace TA_Base_App
{
	class CDataNodeControllerApp : public TA_Base_Bus::TransActiveWinApp
	{
	public:
		CDataNodeControllerApp();


		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CDataNodeControllerApp)				
		//}}AFX_VIRTUAL
	

		//{{AFX_MSG(CDataNodeControllerApp)
			// NOTE - the ClassWizard will add and remove member functions here.
			//    DO NOT EDIT what you see in these blocks of generated code !
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};

} // TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATANODECONTROLLER_H__1ACF5E6B_3440_4FFC_8DAE_034BF0EC2F77__INCLUDED_)
