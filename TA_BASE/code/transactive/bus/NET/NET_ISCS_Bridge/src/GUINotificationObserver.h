/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: $
  * @author 
  * @version $Revision:  $
  * Last modification: $DateTime: $
  * Last modified by:  $Author:  $
  * 
  * This Class is responsible for performing GUI related function such as 
  * ChangeFocus, Move window etc.
  */

#pragma once

using namespace System;
using namespace System::Windows::Forms;

#using <System.Windows.Forms.dll>
#include "IGUINotificationObserver.h"


namespace NET_ISCS_Bridge 
{
	public ref class GUINotificationObserver: IGUINotificationObserver
	{
	public:
		GUINotificationObserver();
		virtual void ChangeFocus(ManagedEFocusType focus);
		virtual void SetPosition(ManagedRECT ^ windowRect,ManagedEWindowPosition windowPos);	
		virtual ManagedRECT^ GetWindowCoordinates();
		void SetMainFrm(Form^ frm);
		virtual bool IsMainFrm();
		virtual void setTopPosition(long top);
	private:
		void ChangeFocusInUIThread(ManagedEFocusType focus);
		delegate void ChangeFocusEvent(ManagedEFocusType focus);

		void SetPositionInUIThread(ManagedRECT ^ windowRect,ManagedEWindowPosition windowPos);
		delegate void SetPositionEvent(ManagedRECT ^ windowRect,ManagedEWindowPosition windowPos);

		Form^ m_pMainFrm;

	};

}