/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: ServerProxy.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for ServerProxy
  */

#ifndef __SERVER_PROXY_H_INCLUDED__
#define __SERVER_PROXY_H_INCLUDED__

#include "ServerConnectionManager.h"
#include "GENAConnectionFactory.h"
#include "CommonDef.h"
#include "GENAMethod.h"
#include <string>

#pragma once

namespace TA_Base_Bus
{
	class ServerProxy
	{
	public:
		ServerProxy(ConnectionFactoryPtr pConnectionMgr);
		~ServerProxy(void);

		std::string subscribe(GENASubscribeMethodPtr objMethod);

		void resubscribe(GENARenewSubscribeMethodPtr objMethod); //const std::string& uri, const unsigned long sid);

		void unsubscribe(GENAUnsubscribeMethodPtr objMethod); //const std::string& uri, const unsigned long& ulSID);

	private: 
		GENARequestStatus _sendRequestInternal(GENARequestPtr pRequest, GenaResponse& objResponse);

	private:
		ConnectionFactoryPtr m_pConnectionMgr;
		ServerConnectionPtr m_pConnection;
	};

} //TA_Base_Bus

#endif //__CLIENT_PROXY_H_INCLUDED__