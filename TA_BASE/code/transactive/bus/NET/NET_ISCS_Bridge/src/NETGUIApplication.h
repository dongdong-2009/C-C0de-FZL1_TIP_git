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
  * This class extends IGUIApplication interface in the ISCS to support
  * DOTNET application to use the exisiting ISCS GUI interfaces.
  */

#pragma once

using namespace System;

#include <vcclr.h>

#include "bus/generic_gui/src/IGUIApplication.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "IProcessStateObserver.h"
#include "IGUINotificationObserver.h"

namespace NET_ISCS_Bridge 
{
	public class WindowPositionInformation
	{
	public:
		WindowPositionInformation()
		{
			posFlag = 0;
			alignFlag=0;
			boundaryDim = TA_Base_Bus::TA_GenericGui::DEFAULT_RECT;
			objectDim= TA_Base_Bus::TA_GenericGui::DEFAULT_RECT;
			appDim=  TA_Base_Bus::TA_GenericGui::DEFAULT_RECT;
		}

		unsigned long posFlag;      // Contains the positioning flags
		unsigned long alignFlag;    // Contains the alignment flags
		RECT boundaryDim;           // The dimension of the boundary to bind the application within
		RECT objectDim;             // The dimension of the object
		RECT appDim;                // The calculated dimension of the object
	};

	public class NETGUIApplication: public TA_Base_Bus::IGUIApplication
	{

	public:

		NETGUIApplication(IProcessStateObserver^ observer	);
		~NETGUIApplication();
		/************** IGUIApplication interface methods *********************************************************/
		virtual void onInitGenericGUICompleted();
		virtual void setGenericGUIStartCompleted(bool bValue);
		virtual bool getGenericGUIStartCompleted() ;		
        virtual unsigned long getApplicationType() ;       
        virtual void checkEntity(TA_Base_Core::IEntityData* guiEntity) ;       
        virtual void checkCommandLine();       
        virtual bool displayApplication();       
        virtual void entityChanged(const std::vector<std::string>& changes);        
        virtual void serverIsDown() ;           
        virtual void serverIsUp();       
        virtual void changeFocus(const TA_Base_Core::EFocusType focus);       
        virtual void setPosition(const unsigned long posFlag,
                               const unsigned long alignFlag,
                               const RECT& objectDim,
                               const RECT& boundaryDim);		
		virtual RECT getWindowCoordinates();         
        virtual void terminateFromControlStation();        
        virtual void dutyChanged();
		/*******************************************************************************************/

		void RegisterGuiNotificationObserver(IGUINotificationObserver^ observer);
		void setWindowPosition();
		void OnMainFrmResize();
	private:
		bool areRectsEqual(const RECT& rect1, const RECT& rect2);
		void moveWindow();
		void moveToWorkArea(RECT& windowRect);
		gcroot<IProcessStateObserver^>  m_ProcessStateObserver;
		gcroot<IGUINotificationObserver^> m_GuiNotificationObserver;
		bool m_genericGUIStartCompleted;

		WindowPositionInformation m_positionInfo;
		bool m_bFirst;

		RECT m_workingArea;

	};
}