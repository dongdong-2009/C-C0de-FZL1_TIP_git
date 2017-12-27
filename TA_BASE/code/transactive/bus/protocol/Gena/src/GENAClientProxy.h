/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GENAClientProxy.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for ClientProxy
  */

#ifndef __CLIENT_PROXY_H_INCLUDED__
#define __CLIENT_PROXY_H_INCLUDED__

#include "GENAHttpClient.h"
#include "CommonDef.h"
#include "GENATopicData.h"
#include "GENAConnectionFactory.h"
#include "core/threads/src/QueueProcessor.h"
#include "bus/protocol/Gena/src/GENARequest.h"

#pragma once

namespace TA_Base_Bus
{
	class GENAClientProxy
		: public TA_Base_Core::QueueProcessor<GENATopicData>
	{
	public:
		GENAClientProxy(SubscriberInfoPtr objSubInfo, ConnectionFactoryPtr pConnectionMgr);
		~GENAClientProxy(void);

		std::string getCallbackAddress();

		std::string getSID();

	protected:
		/**
		 * QueueProcessor::processEvent()
		 *
		 * Adds the new event request to the current bundle
		 *
		 * @param newEvent	the request for the new event
		 *
		 */
		virtual void processEvent( boost::shared_ptr<GENATopicData> ptopicUpdate );
	
	private: // Methods
		//void notifyUpdate(GENATopicDataPtr ptopicUpdate);

	private:
		std::string m_sid;
		std::string m_hostName;
		std::string m_callbackAddress;
		ConnectionFactoryPtr m_pConnectionMgr;
		ConnectionPtr m_pConnection;
	};

} //TA_Base_Bus

#endif //__CLIENT_PROXY_H_INCLUDED__