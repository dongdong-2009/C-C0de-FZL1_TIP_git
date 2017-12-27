#include "MySink.h"
#include "ATSComWrapper.h"
#include "ATSRealTimeWorker.h"
#include "ATSRealTimeObserverManager.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/CombinedEventData.h"

using namespace TA_Base_Core;
using namespace TA_Base_App;

CMySink::CMySink(void)
{
	EnableAutomation();
}

CMySink::~CMySink(void)
{
}
// CSink

IMPLEMENT_DYNAMIC(CMySink, CCmdTarget)




void CMySink::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CMySink, CCmdTarget)
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CMySink, CCmdTarget)
	DISP_FUNCTION(CMySink, "EventUpdate", OnEventUpdate, VT_I4, VTS_I4 VTS_UNKNOWN)
END_DISPATCH_MAP()

// Note: we add support for IID_ISink to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .IDL file.

// {51CF45A6-FD10-4F51-8618-AD03C9207F62}
//static const IID IID_ISink =
//{ 0x51CF45A6, 0xFD10, 0x4F51, { 0x86, 0x18, 0xAD, 0x3, 0xC9, 0x20, 0x7F, 0x62 } };

BEGIN_INTERFACE_MAP(CMySink, CCmdTarget)
	INTERFACE_PART(CMySink, DIID__IATSEventSubscriptionEvents, Dispatch)
END_INTERFACE_MAP()


// CMySink message handlers

HRESULT CMySink::OnEventUpdate(long error, IATSEventList* pATSEvents)
{
	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Come into function eventSubscriptionCallback(). ");
 
 	if(error == 0 /*ATS_SUBSCRIPTION_OK*/)
	{
 
 		try{
 			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ATS Real-time :Get  %d ATS refreshing data.", pATSEvents->GetSize());
 			std::vector<TA_Base_Core::ICombinedEventData*> events;	
 
			ATSComWrapper::getInstanceRealTime()->transformToISCSEvents ( pATSEvents, events, ATSRealTimeWorker::getCurrentMaxKey() );
 
 			ATSRealTimeWorker::setCurrentMaxKey( ATSRealTimeWorker::getCurrentMaxKey() + events.size());
 
 			ATSRealTimeObserverManager::getInstance()->notifyObserverList(TA_Base_App::NoCommand, events, ATSRealTimeObserverManager::getInstance()->getTaskID());
 		}
 		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception caught in ATS callback eventSubscriptionCallback().");
		}

		if (pATSEvents)
		{
			pATSEvents->Release();
		}
	}
	else  
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get ATSSubscriptionError:%d in ATSEventWrapper::eventSubscriptionCallback.", error);

		ATSRealTimeObserverManager::getInstance()->setATSSubscriptionErr(error);

	}

	return E_NOTIMPL;
}
