/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GenaServerImp.h  $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for GenaServerImp
  */

#ifndef __GENA_SERVER_H_INCLUDED__
#define __GENA_SERVER_H_INCLUDED__

#include "CommonDef.h"
#include "GENATopicData.h"
#include "GENAConnectionFactory.h"
#include "core/threads/src/Thread.h"
#pragma once

namespace TA_Base_Bus
{
	//Forward Declaration
	class GenaServerImp;
	class GENAHttpServer;
	class IGENAClientStatusForwarder;
	class IGENATopicDataStore;

	class GenaServer
		: private TA_Base_Core::Thread
	{
	public:
		GenaServer(const GenaServerParams& pServerParams);
		~GenaServer();

		void startService();

		void stopService();

		void destroyService();
		
		void publishUpdate(GENATopicDataPtr ptopicUpdate);

		void registerDataStore(IGENATopicDataStore* pDataStore);

		void registerForwarder(IGENAClientStatusForwarder* pRequestFwd);

		void registerClientStatusObserver();

		ConnectionFactoryPtr getConnectionMgr() { return m_pConnectionMgr; };

	protected:
		virtual void run();
		virtual void terminate();

	private:
		GenaServerImp* m_pServerImp;
		GENAHttpServer* m_pHttpServer;
		ConnectionFactoryPtr m_pConnectionMgr;
	};

	typedef boost::shared_ptr<GenaServer> GenaServerPtr;

} //TA_Base_Bus

#endif //__GENA_SERVER_H_INCLUDED__
