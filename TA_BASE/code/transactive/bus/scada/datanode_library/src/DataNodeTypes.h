/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datanode_library/src/DataNodeTypes.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 14:56:06 $
  * Last modified by:  $Author: haijun.li $
  *
  * DataNode is an abstract base class representing
  * a single value of any supported type. It maintains
  * a set of rules determining whether or not the value
  * can be updated, as well as a set of alarm conditions.
  * DataNode also realises the DataNodeCorbaDef interface,
  * exposing the DataNode's configuration to remote 
  * clients.
  */

#ifndef DATANODETYPES_H
#define DATANODETYPES_H

#include <string>
#include <vector>
#include <map>
#include "core/naming/src/NamedObject.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/common_library/IDL/src/IDataPointAccessCorbaDef.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"

namespace TA_Base_Bus
{
    typedef std::map<unsigned long, DataPointProxySmartPtr*> ChildDataPointMap;
    typedef ChildDataPointMap::iterator ChildDataPointMapIt;

    typedef std::vector<DataNodeProxySmartPtr*> ChildDataNodeVector;
    typedef ChildDataNodeVector::iterator ChildDataNodeVectorIt;

    typedef std::map<unsigned long, DataNodeProxySmartPtr*> ChildDataNodeMap;
    typedef ChildDataNodeMap::iterator ChildDataNodeMapIt;


	//xinsong++
	//encapsulate function that access DataNodeChildProxy
	class DataNodeChildProxies
	{
	public:
		DataNodeChildProxies()
		{
			m_dataNodesHaveValidConfig = false;
			m_dataPointsHaveValidConfig = false;
		}

		~DataNodeChildProxies()
		{
			freeChildProxy();		
		}

		void freeChildProxy();

		bool checkChilidProxiesCreated(TA_Base_Bus::DataNodeStaticConfiguration & staticConfig,
			TA_Base_Bus::ScadaProxyFactory& scadaProxyFactory, IEntityUpdateEventProcessor & entityEventProcessor);

		ChildDataNodeMap & getDataNodes()
		{
			return m_dataNodes;
		}

		ChildDataPointMap & getDataPoints()
		{
			return m_dataPoints;
		}

		bool isChildDataPointProxyCreated()
		{
			if ( !m_dataPointsHaveValidConfig )
			{
				checkConfigValidForChildDataPoints();
			}
			return m_dataPointsHaveValidConfig;
		}

		bool isChildDataNodeProxyCreated()
		{
			if ( !m_dataNodesHaveValidConfig )
			{
				checkConfigValidForChildDataNodes();
			}
			return m_dataNodesHaveValidConfig;
		}

	private:
		DataPointProxySmartPtr* getChildDataPointProxy(unsigned long entityKey);
		DataNodeProxySmartPtr* getChildDataNodeProxy(unsigned long entityKey);
		bool checkConfigValidForChildDataPoints();
		bool checkConfigValidForChildDataNodes();
		void freeChildDataNodeProxy();
		void freeChildDataPointProxy();

	private:
		// State information for the internal workings of the summaries
        //ChildDataNodeVector						m_dataNodes;
		ChildDataNodeMap                        m_dataNodes;
        bool                                    m_dataNodesHaveValidConfig;

        ChildDataPointMap						m_dataPoints;
        bool                                    m_dataPointsHaveValidConfig;
		
	};

    typedef TA_Base_Core::NamedObject< IDataPointAccessCorbaDef,
                                       IDataPointAccessCorbaDef_ptr,
                                       IDataPointAccessCorbaDef_var >   DataPointAgentInterface;	
    
    
}
#endif
