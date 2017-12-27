/**
  * The source code in this file is the property of
  * Combuilder PTE LTD and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/banner_framework/src/PrintButton.cpp $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/07/02 09:30:42 $
  * Last modified by:  $Author: Shiva $
  *
  * This class represents a print button that can be added
  * to the banner.
  *
  */

#include "stdafx.h"

#include "bus/resources/resource.h"
#include "core/utilities/src/DebugUtil.h"

#include "bus/banner_framework/src/PrintButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{
	BEGIN_MESSAGE_MAP(PrintButton, CButton)
		//{{AFX_MSG_MAP(PrintButton)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	void PrintButton::intialize()
	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_BANNER_PRINT_ICON), IMAGE_ICON, 20, 20, LR_LOADMAP3DCOLORS);
		SetIcon(hIcon);		
	}

	void PrintButton::setPrintableObject(IPrintableObject* pPrintObject)
	{
		if (NULL != pPrintObject)
		{
			m_pPrintableObject = pPrintObject;
		}
	}

	void PrintButton::print()
	{
		if (NULL != m_pPrintableObject)
		{
			m_pPrintableObject->printObject();
		}
	}

}