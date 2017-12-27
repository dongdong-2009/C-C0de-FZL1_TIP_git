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
  * This Interface is providing GUI related function such as 
  * ChangeFocus, Move window etc.
  */

#pragma once
using namespace System;

namespace NET_ISCS_Bridge 
{

	public enum class ManagedEFocusType
	{
		GetFocus, 
		Minimise, 
		Maximise
	};

	public enum class ManagedEWindowPosition       // The new position of the window
	{
		MINIMISE,               // Minimise the app
		MAXIMISE,               // Maximise the app
		FOCUS,                  // Give focus to the app
		REPOSITION,             // Reposition the app
		NO_REPOSITION,          // This is the same as FOCUS but this is used in launchApplication call rather than getFocus call
		CENTRE,                 // Centre the app
		REPOSITION_NO_RESTORE
	};

	public ref class ManagedRECT
	{
	public:
		ManagedRECT()
		{
			left = -1;
			top= -1;
			right= -1;
			bottom = -1;
		}
		long left;
		long top;
		long right;
		long bottom;
	};

	public interface class IGUINotificationObserver
	{
		void ChangeFocus(ManagedEFocusType focus);
		void SetPosition(ManagedRECT ^ windowRect,ManagedEWindowPosition windowPos);	
		ManagedRECT^ GetWindowCoordinates();
		void  setTopPosition(long top);
		bool IsMainFrm();
	};


}