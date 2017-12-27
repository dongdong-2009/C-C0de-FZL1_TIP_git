///////////////////////////////////////////////////////////
//  ScadaDataProxy.h
//  Implementation of the Class ScadaDataProxy
//  Created on:      13-Ê®¶þÔÂ-2010 11:16:03
//  Original author: luohuirong
///////////////////////////////////////////////////////////
#if !defined(ScadaDataProxy_BF44_FE96B5E5B076__INCLUDED_)
#define ScadaDataProxy_BF44_FE96B5E5B076__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"


  

namespace TA_Base_Bus
{
	class DataPointProxySmartPtr;
	class ScadaProxyFactory;
	class ScadaDataProxy : public TA_Base_Bus::IEntityUpdateEventProcessor
	{
	public:
		
		static ScadaDataProxy* getInstance();
		static void  removeInstance();
		
		virtual ~ScadaDataProxy(void);

		/**
		* processEntityUpdateEvent
		*
		* Implement the interface of IEntityUpdateEventProcessor.
		*/
		void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

		bool addDataPoint(unsigned long entityKey);

		bool delDataPoint(unsigned long entityKey);

		bool releaseAllDataPoints();

		bool getDataPointValue(unsigned long entityKey,double& value);


	private:		 
        ScadaDataProxy(void);
		static ScadaDataProxy* m_instance;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

		TA_Base_Bus::ScadaProxyFactory* m_proxyFactory;
	 

		typedef std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*> DataPointProxyMap;
		TA_Base_Core::ReEntrantThreadLockable m_dataPointMapLock;

		DataPointProxyMap m_dataPoints;

	};
	
}
#endif // !defined(ScadaDataProxy_BF44_FE96B5E5B076__INCLUDED_)
