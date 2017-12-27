// DutyManagerBridge.cpp: implementation of the DutyManagerBridge class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "app/security/DutyManager/src/DutyManager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace TA_Base_Bus;

namespace TA_Base_App
{
	
	DutyManagerBridge::DutyManagerBridge(DutyManagerDlg * parentWnd, const std::string & originalTitleText)
		:ISessionObserver(DutyDataStore::getSessionCache()),
		IConfigDataObserver(DutyDataStore::getDataCache()),
		m_dataCache(DutyDataStore::getDataCache()),
		m_sessionCache(DutyDataStore::getSessionCache()),
		m_originalTitleText(originalTitleText),
		m_isCentralProfile(true)
	{
		this->m_parentWnd = parentWnd;
	}
	
	DutyManagerBridge::~DutyManagerBridge()
	{
		//DutyDataStore::cleanUp();
	}
	
	void DutyManagerBridge::dataChanged(const TA_Base_Bus::DataUpdate& dataUpdate)
	{
		TA_ASSERT(m_parentWnd != NULL, "Parent window is NULL!");
		typedef std::map<TA_Base_Core::EModificationType, std::string> ActionMap;
		typedef std::map<TA_Base_Core::EObjectType, std::string> ItemMap;
		
		static ActionMap actionMap;
		static ItemMap itemMap;
		
		if (actionMap.empty())
		{
			actionMap.insert(std::make_pair(TA_Base_Core::Create, std::string("added")));
			actionMap.insert(std::make_pair(TA_Base_Core::Update, std::string("modified")));
			actionMap.insert(std::make_pair(TA_Base_Core::Delete, std::string("deleted")));
		}
		
		if (itemMap.empty())
		{
			itemMap.insert(std::make_pair(TA_Base_Core::OPERATOR, std::string("Operator")));
			itemMap.insert(std::make_pair(TA_Base_Core::PROFILE, std::string("Profile")));
			itemMap.insert(std::make_pair(TA_Base_Core::ACTION_GROUP, std::string("Action group")));
			itemMap.insert(std::make_pair(TA_Base_Core::LOCATION, std::string("Location")));
			itemMap.insert(std::make_pair(TA_Base_Core::REGION, std::string("Region")));
			itemMap.insert(std::make_pair(TA_Base_Core::SUBSYSTEM, std::string("Subsystem")));
		}
		
		ActionMap::const_iterator actionIt(actionMap.find(dataUpdate.updateType));
		ItemMap::const_iterator itemIt(itemMap.find(dataUpdate.itemType));
		
		if (actionIt == actionMap.end() || itemIt == itemMap.end())
		{
			return;
		}
		
		//std::ostringstream msg;
		//msg << itemIt->second << " configuration data has been " << actionIt->second << "." << std::endl
		//	<< TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME) << " should be restarted for any" << std::endl
		//	<< "changes to become visible.";

		//MessageBox::warning(msg.str());

		TA_Base_Bus::TransActiveMessage userMsg;
		userMsg<<itemIt->second;
		userMsg<<actionIt->second;
		userMsg<<TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME);
		userMsg.showMsgBox(IDS_UW_670006, "权限管理器");
	}
	
	void DutyManagerBridge::sessionChanged(const TA_Base_Bus::SessionUpdate& sessionUpdate)
	{
		// TD11803: A call to SendMessage was made when the window did not exist, for now just make sure
		// it won't generate an assertion and log the fact so it can be debugged further.
		if (::IsWindow(m_parentWnd->m_hWnd) )
		{
			// Let the thread that created the window process the update.
			OnReceiveSessionUpdate(sessionUpdate);
		}
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Attempted to SendMessage when window does not exist." );
		}
	}

	void DutyManagerBridge::initialiseCurrentSession()
	{	
		USES_CONVERSION;
		TA_ASSERT(m_parentWnd != NULL, "Parent window is NULL!");
		try
		{
			SessionInfoEx sessionInfoEx(m_sessionCache.getSessionInfo(m_sessionCache.getCurrentSessionId()));
			
			// TD11122: Operator/Profile information is now placed in the title bar.
			std::ostringstream titleText;
			
			// Application name.
			titleText << m_originalTitleText << " - ";
			
			// Base operator and profile.
			titleText << m_dataCache.getOperatorName(sessionInfoEx.OperatorId[0]) << "/"
				<< m_dataCache.getProfileName(sessionInfoEx.ProfileId[0]);

			if ( !m_dataCache.isCentralProfile(sessionInfoEx.ProfileId[0]) )
			{
				m_isCentralProfile = false;
			}
			
			TA_ASSERT( sessionInfoEx.OperatorId.size()==sessionInfoEx.ProfileId.size(), "Operator <-> Profile mismatch!" );
			
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"DutyManagerBridge::initialiseCurrentSession, sessionInfoEx.OperatorId.size() = %d",sessionInfoEx.OperatorId.size());
			// Remaining operators and profiles.
			for ( unsigned int i=1; i<sessionInfoEx.OperatorId.size(); i++ )
			{
				// There is always at least one profile at this point so we always include the comma.2
				titleText << ", " << m_dataCache.getOperatorName(sessionInfoEx.OperatorId[i]) << "/"
					<< m_dataCache.getProfileName(sessionInfoEx.ProfileId[i]);
			}
			CA2T title(titleText.str().c_str());
			m_parentWnd->SetWindowText( title);
			
			return;
		}
		catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
		{
			//MessageBox::error("Error while initialising current session details.", ex);
			MessageBox::error("",ex,IDS_UW_670007);
		}
		catch (const TA_Base_Core::TransactiveException& ex)
		{
			MessageBox::error("",ex,IDS_UW_670007);
		}
		catch (...)
		{
			MessageBox::error("",IDS_UW_670007);
		}   
		CA2T originalTitle(m_originalTitleText.c_str());
		m_parentWnd->SetWindowText( originalTitle );
	}

	void DutyManagerBridge::OnReceiveSessionUpdate(const TA_Base_Bus::SessionUpdate& sessionUpdate)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"begin DutyManagerBridge::OnReceiveSessionUpdate");
		// If current session undergoes a profile change or a profile override, 
		// the details of the session displayed by the dialog must be updated.
		
		if (sessionUpdate.updateType == TA_Base_Bus::TA_Security::SessionEnd ||
			sessionUpdate.sessionId != m_sessionCache.getCurrentSessionId())
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"updateType==SessionEnd or not current session, so end DutyManagerBridge::OnReceiveSessionUpdate");

			return ;
		}
		
		initialiseCurrentSession();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"end DutyManagerBridge::OnReceiveSessionUpdate");
	}	

}