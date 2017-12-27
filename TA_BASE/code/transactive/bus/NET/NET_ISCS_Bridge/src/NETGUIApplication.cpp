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

#include "stdafx.h"

#include "NETGUIApplication.h"
#include "bus\generic_gui\src\AppLauncher.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core\utilities\src\DebugUtil.h"

using namespace TA_Base_Bus::TA_GenericGui;
using TA_Base_Core::DebugUtil;

namespace NET_ISCS_Bridge 
{

	NETGUIApplication::NETGUIApplication(IProcessStateObserver^ observer)
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "NETGUIApplication Constructor" );
		m_ProcessStateObserver = observer;
		m_genericGUIStartCompleted = false;
		m_GuiNotificationObserver = nullptr;
		m_bFirst = true;
		LOG( SourceInfo, DebugUtil::FunctionExit, "NETGUIApplication Constructor" );
	}

	NETGUIApplication::~NETGUIApplication()
	{
		m_GuiNotificationObserver = nullptr;
	}

	void NETGUIApplication::onInitGenericGUICompleted()
	{
		if(m_ProcessStateObserver)
		{
			m_ProcessStateObserver->onInitGenericGUICompleted();
		}
	}

	void NETGUIApplication::setGenericGUIStartCompleted(bool bValue)
	{
		m_genericGUIStartCompleted = bValue;
	}

	bool NETGUIApplication::getGenericGUIStartCompleted()
	{
		return m_genericGUIStartCompleted;
	}

	unsigned long NETGUIApplication::getApplicationType()
	{
		return m_ProcessStateObserver->GetApplicationtype();
	}

	void NETGUIApplication::checkEntity(TA_Base_Core::IEntityData* guiEntity)
	{
		return m_ProcessStateObserver->setGuiEntity(guiEntity);
	}

	void NETGUIApplication::checkCommandLine()
	{
		//TODO
	}

	bool NETGUIApplication::displayApplication()
	{
		//TODO
		return true;
	}

	void NETGUIApplication::entityChanged(const std::vector<std::string>& changes)
	{
		//TODO
	}

	void NETGUIApplication::serverIsDown()
	{
		if(m_ProcessStateObserver)
		{
			m_ProcessStateObserver->ServerStateChange(false);
		}
	}          
	void NETGUIApplication::serverIsUp()
	{
		if(m_ProcessStateObserver)
		{
			m_ProcessStateObserver->ServerStateChange(true);
		}
	}       

	void NETGUIApplication::changeFocus(const TA_Base_Core::EFocusType focus)
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "changeFocus" );
		try
		{
			ManagedEFocusType managedFocus= static_cast<ManagedEFocusType>(focus);
			if(m_GuiNotificationObserver)
			{
				m_GuiNotificationObserver->ChangeFocus(managedFocus);
			}
		}
		catch(...)
		{					
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,"Exception Catched" );			
		}
		LOG( SourceInfo, DebugUtil::FunctionExit, "changeFocus" );
	}  

	void NETGUIApplication::setPosition(const unsigned long posFlag,
		const unsigned long alignFlag,
		const RECT& objectDim,
		const RECT& boundaryDim)
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "setPosition" );

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Received PosFlag %d, AlignFlag %d, objectDim - (%li,%li,%li,%li) and boundaryDim=  (%li,%li,%li,%li)", posFlag,alignFlag,objectDim.left, objectDim.top, objectDim.right, objectDim.bottom,boundaryDim.left, boundaryDim.top, boundaryDim.right, boundaryDim.bottom);

		// The GUI should just be given focus.
		if (posFlag == TA_Base_Bus::TA_GenericGui::POS_NONE)
		{
			//LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Will give focus to GUI, rather than moving it");
			if(m_GuiNotificationObserver)
			{
				m_GuiNotificationObserver->ChangeFocus(ManagedEFocusType::GetFocus);
			}

			LOG( SourceInfo, DebugUtil::FunctionExit, "setPosition" );
			return;
		}

		if (((posFlag != TA_Base_Bus::TA_GenericGui::POS_NONE) && (m_positionInfo.posFlag != posFlag)) ||
			(m_positionInfo.alignFlag != alignFlag) ||
			(!areRectsEqual(m_positionInfo.objectDim, objectDim)) ||
			(!areRectsEqual(m_positionInfo.boundaryDim, boundaryDim)))
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Setting m_positionInfo values");
			m_positionInfo.posFlag = posFlag;
			m_positionInfo.alignFlag = alignFlag;
			m_positionInfo.objectDim = objectDim;
			m_positionInfo.boundaryDim = boundaryDim;
			// reset the application dimension
			m_positionInfo.appDim = TA_Base_Bus::TA_GenericGui::DEFAULT_RECT;
			setWindowPosition();		

			LOG( SourceInfo, DebugUtil::FunctionExit, "setPosition" );
			return;
		}

		// if the new position is exactly the same as the old one, then we don't have
		// to recalculate the position again.  We'll only move the window if it's equal
		// to the default rect (which is what we set the appDim to when the application is
		// not meant to be repositioned)
		moveWindow();

		LOG( SourceInfo, DebugUtil::FunctionExit, "setPosition" );

	}	


	bool NETGUIApplication::areRectsEqual(const RECT& rect1, const RECT& rect2)
	{
		return ((rect1.left == rect2.left) &&
			(rect1.right == rect2.right) &&
			(rect1.top == rect2.top) &&
			(rect1.bottom == rect2.bottom));
	}

	void NETGUIApplication::setWindowPosition()
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "setWindowPosition" );

		// If the application is null then we are trying to move it before it has
		// set up and initialised correctly. We will just return.
		if (!(m_GuiNotificationObserver->IsMainFrm()))
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,"setWindowPosition() was called on the application before it was initialised correctly. It will not change focus");
			LOG( SourceInfo, DebugUtil::FunctionExit, "setWindowPosition" );
			return;
		}
		RECT windowRect = getWindowCoordinates();
		bool isRepositioned = TA_Base_Bus::AppLauncher::getInstance().calculateNewCoordinates(windowRect, 
			m_positionInfo.objectDim, 
			m_positionInfo.boundaryDim, 
			m_positionInfo.posFlag, 
			m_positionInfo.alignFlag);

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Calculated new coordinates : (%li,%li,%li,%li) and isRespositioned = %d", windowRect.left, windowRect.top, windowRect.right, windowRect.bottom,isRepositioned);
		// stores the new window dimension
		m_positionInfo.appDim = isRepositioned ? windowRect : TA_Base_Bus::TA_GenericGui::DEFAULT_RECT;
		moveWindow();
		m_bFirst = false;

		LOG( SourceInfo, DebugUtil::FunctionExit, "setWindowPosition" );
	}


	void NETGUIApplication::moveWindow()
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "moveWindow" );

		//Maochun Sun++
		//TD12779
		if (!m_GuiNotificationObserver && !(m_GuiNotificationObserver->IsMainFrm()))
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
				"moveWindow() was called on the application before it was initialised correctly.");
			LOG( SourceInfo, DebugUtil::FunctionExit, "moveWindow" );
			return;
		}
		//++Maochun Sun
		//TD12779		

		// if its coordinates are changed, then move to the new coordinates, otherwise, centre the
		// application without changing the size of the application.
		if (!areRectsEqual(m_positionInfo.appDim, TA_Base_Bus::TA_GenericGui::DEFAULT_RECT))
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "moveWindow rect are not equal to default_rect");
			RECT& windowRect = m_positionInfo.appDim;
			// Have to use post message rather than send message 'cos we want it to fully repositioned itself
			// before giving it focus.
			ManagedRECT^ rect = gcnew ManagedRECT();
			rect->top = windowRect.top;
			rect->bottom = windowRect.bottom;
			rect->left = windowRect.left;
			rect->right = windowRect.right;
			ManagedEWindowPosition windPos = static_cast<ManagedEWindowPosition>(REPOSITION);
			m_GuiNotificationObserver->SetPosition(rect, windPos);
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Moved to: (%li,%li,%li,%li)", windowRect.left, windowRect.top, windowRect.right, windowRect.bottom);
			LOG( SourceInfo, DebugUtil::FunctionExit, "moveWindow" );
			return;
		}
		ManagedRECT^ rect = gcnew ManagedRECT();
		// send the set window message to the main window thread.  Have to wait for the set 
		// window position method to finish 'cos we want the new position
		ManagedEWindowPosition windPos;
		if(m_bFirst)
			windPos = static_cast<ManagedEWindowPosition>(CENTRE);
		else
			windPos = static_cast<ManagedEWindowPosition>(NO_REPOSITION);
		m_GuiNotificationObserver->SetPosition(rect, windPos);
		LOG( SourceInfo, DebugUtil::FunctionExit, "moveWindow" );
	}


	RECT NETGUIApplication::getWindowCoordinates()
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "getWindowCoordinates" );
		RECT unmanagedRect;
		if(m_GuiNotificationObserver)
		{
			ManagedRECT^ rect = m_GuiNotificationObserver->GetWindowCoordinates();
			unmanagedRect.left = rect->left;
			unmanagedRect.top = rect->top;
			unmanagedRect.bottom = rect->bottom;
			unmanagedRect.right=rect->right;
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Got Coordinates as to: (%li,%li,%li,%li)", unmanagedRect.left, unmanagedRect.top, unmanagedRect.right, unmanagedRect.bottom);
		}
		LOG( SourceInfo, DebugUtil::FunctionExit, "getWindowCoordinates" );
		return unmanagedRect;
	}  


	void NETGUIApplication::terminateFromControlStation()
	{
		if(m_ProcessStateObserver)
		{
			m_ProcessStateObserver->PreTerminateApplication(TA_Base_Core::RequestedTerminate);
		}
	}       


	void NETGUIApplication::dutyChanged()
	{
		//todo in future
	}


	void NETGUIApplication::RegisterGuiNotificationObserver(IGUINotificationObserver^ observer)
	{
		m_GuiNotificationObserver = observer;
	}

	void NETGUIApplication::OnMainFrmResize()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "----------OnWindowPosChanging_Entry");		
		ManagedRECT^ rect = m_GuiNotificationObserver->GetWindowCoordinates();
		try
        {
			m_workingArea = TA_Base_Bus::AppLauncher::getInstance().getRect(SCREEN_CURRENT, AREA_SCHEMATIC, (int) rect->left);
			LOG4(SourceInfo, DebugUtil::DebugInfo, "----------X=%d,Y=%d,CX=%d,CY=%d",m_workingArea.left,m_workingArea.top,m_workingArea.bottom,m_workingArea.right);
        }
        catch(TA_Base_Core::ApplicationException& ex)
        {
            // Log the catch, and set the workingArea rect to 0.
            std::stringstream errorMessage;
            errorMessage << "An exception was caught while attempting to retrieve the schematic area of the current"
                << "screen. X coord = " << rect->left << ". Error Message:";
            LOG_EXCEPTION_CATCH(SourceInfo,"ApplicationException",errorMessage.str().c_str());
            LOGMORE(SourceInfo,ex.what());
			
            // Set the workingArea rect to 0
			m_workingArea.left = 0;
			m_workingArea.bottom=0;
			m_workingArea.right=0;
			m_workingArea.top=0;
        }

		RECT windowRect;
		moveToWorkArea(windowRect);	
		//we need only the top position, to limited the window in the work area always
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "----------Top position Y=%d",m_workingArea.top);
		if ( windowRect.top != -1 )
		{
			m_GuiNotificationObserver->setTopPosition(windowRect.top);
		}
	}

	void NETGUIApplication::moveToWorkArea(RECT& windowRect)
	{
		FUNCTION_ENTRY("moveToWorkArea");

		RECT newWindowRect;
		ManagedRECT^ windowOldRect = m_GuiNotificationObserver->GetWindowCoordinates();
		System::Drawing::Point^ cursorPos = System::Windows::Forms::Control::MousePosition;

		int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		int captionHeight = GetSystemMetrics(SM_CYCAPTION);
		int SingleScreenWidth = 1280;		
		int currentScreen = (cursorPos->X + 1)/SingleScreenWidth;
		int windowHeight = windowOldRect->bottom - windowOldRect->top;
		
		//the follow line is center window to the screen which cursor in
		//newWindowRect.left = (currentScreen * SingleScreenWidth ) + ((SingleScreenWidth - (windowOldRect.right - windowOldRect.left)) / 2);		
		
		if( 0 == m_workingArea.top && 0 == m_workingArea.left && 0 == m_workingArea.right && 0 == m_workingArea.bottom)
		{
			newWindowRect.top = -1;
		}
		else if(cursorPos->Y < m_workingArea.top ||  cursorPos->Y - captionHeight < m_workingArea.top)
		{
			newWindowRect.top = m_workingArea.top;
		}
		else if(cursorPos->Y + windowHeight > m_workingArea.bottom || cursorPos->Y + windowHeight + captionHeight > m_workingArea.bottom )
		{
			newWindowRect.top = m_workingArea.bottom - windowHeight;
		}
		else
		{
			newWindowRect.top = -1;
		}

		newWindowRect.left = windowOldRect->left;
		newWindowRect.right = newWindowRect.left + (windowOldRect->right - windowOldRect->left);
		newWindowRect.bottom = newWindowRect.top + (windowOldRect->bottom - windowOldRect->top);		
		
		windowRect.left = newWindowRect.left;
		windowRect.top = newWindowRect.top;
		windowRect.right = newWindowRect.right;
		windowRect.bottom = newWindowRect.bottom;

		FUNCTION_EXIT;
	}
}
