/**
  * The source code in this file is the property of
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/banner_framework/src/PrintButton.h $
  * @author:  Noel Tajanlangit
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This class represents a print button which does a screen capture and calls the print dialog
  * for printing.
  *
  */

#if !defined(__PRINT_BUTTON_H_INCLUDED__)
#define __PRINT_BUTTON_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/banner_framework/src/IPrintableObject.h"

namespace TA_Base_Bus
{

	class PrintButton : public CButton
	{
	public:
		PrintButton() : m_pPrintableObject(0) {};

		void intialize();

		void setPrintableObject(IPrintableObject* pPrintObject);

		void print();

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(SilenceButton)
		//}}AFX_VIRTUAL
		virtual ~PrintButton() {};

		// Generated message map functions
	protected:
		//{{AFX_MSG(PrintButton)
		// NOTE - the ClassWizard will add and remove member functions here.
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()

	private:
		IPrintableObject* m_pPrintableObject;

	};
}

#endif // __PRINT_BUTTON_H_INCLUDED__