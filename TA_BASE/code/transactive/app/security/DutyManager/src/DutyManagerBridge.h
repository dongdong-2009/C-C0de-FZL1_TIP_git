// DutyManagerBridge.h: interface for the DutyManagerBridge class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUTYMANAGERBRIDGE_H__837F9373_19F4_4EBD_BD6C_BA7584C234AB__INCLUDED_)
#define AFX_DUTYMANAGERBRIDGE_H__837F9373_19F4_4EBD_BD6C_BA7584C234AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/security/duty_monitor_framework/src/ISessionObserver.h"
#include "bus/security/duty_monitor_framework/src/IConfigDataObserver.h"

namespace TA_Base_App
{
	class DutyManagerDlg;
	class DutyManagerBridge: public  TA_Base_Bus::ISessionObserver,
		public TA_Base_Bus::IConfigDataObserver//*/
	{
	public:
		DutyManagerBridge(DutyManagerDlg * parentWnd, const std::string & originalTitleText);
		virtual ~DutyManagerBridge();
        // ISessionObserver interface
        virtual void sessionChanged(const TA_Base_Bus::SessionUpdate& sessionUpdate);
		
        // IConfigDataObserver interface
        virtual void dataChanged(const TA_Base_Bus::DataUpdate& dataUpdate);


		void initialiseCurrentSession();
	private:
		 DutyManagerBridge(DutyManagerBridge &);
		 DutyManagerBridge& operator = (DutyManagerBridge &);
		
		 void OnReceiveSessionUpdate(const TA_Base_Bus::SessionUpdate& sessionUpdate);

	private:
		DutyManagerDlg * m_parentWnd;
        TA_Base_Bus::DataCache& m_dataCache;
        TA_Base_Bus::SessionCache& m_sessionCache;
		std::string m_originalTitleText;
		bool m_isCentralProfile;
	};
}
#endif // !defined(AFX_DUTYMANAGERBRIDGE_H__837F9373_19F4_4EBD_BD6C_BA7584C234AB__INCLUDED_)
