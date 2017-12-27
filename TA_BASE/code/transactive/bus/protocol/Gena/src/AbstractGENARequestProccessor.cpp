
#include "AbstractGENARequestProccessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "Poco/Net/HTTPResponse.h"
#include "ServerModeException.h"
#include "SIDNotFoundException.h"

using Poco::Net::HTTPResponse;

namespace TA_Base_Bus
{

	AbstractGENARequestProccessor::AbstractGENARequestProccessor(void)
	{
	}

	AbstractGENARequestProccessor::~AbstractGENARequestProccessor(void)
	{
	}

	void AbstractGENARequestProccessor::proccessRequest(GENARequestPtr objRequest, GenaResponse& objResponse)
	{
		FUNCTION_ENTRY("proccessRequest");

		try
		{
			if ( objRequest->getMethod() == REQ_SUBSCRIBE)
			{
				LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Subscribe Request");
				
				std::string szSID = subscribe(objRequest);
				if (objRequest->getHeaderValue(HEAD_SID).empty()) //not renew subscriber
				{					 
					objResponse.msgHeader.set(HEAD_SID, szSID);
				}
				// response status
				objResponse.status = GENA_OK;
			}
			else if (objRequest->getMethod() == REQ_UNSUBSCRIBE)
			{
				LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Unsubscribe Request");
				unsubscribe(objRequest);

				objResponse.status = GENA_OK;
			}
			else if (objRequest->getMethod() == REQ_NOTIFY)
			{
				notify(objRequest);
				objResponse.status = GENA_OK;
				LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Notify Request was made.");
			}
			else
			{
				LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unsupported Request was made! Please check request: %s", objRequest->getMethod().c_str());
				objResponse.status = GENA_BAD_REQUEST;
			}

			
		}
		catch(const TA_Base_Core::ServerModeException& ex)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Operation mode exception occurred! %s", ex.what());
			objResponse.status = GENA_PASSIVE_MODE;
		}
		catch(const TA_Base_Core::SIDNotFoundException& ex)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SID exception occurred! %s", ex.what());
			objResponse.status = GENA_SID_NOT_FOUND;
		}
		catch(...)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occured while processing request");
			objResponse.status = GENA_BAD_REQUEST;
		}
		FUNCTION_EXIT;
	}

	
} //TA_Base_Bus

