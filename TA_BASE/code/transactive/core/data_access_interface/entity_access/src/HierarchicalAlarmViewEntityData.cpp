/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/HierarchicalAlarmViewEntityData.cpp $
  * @author Lizette Lingo
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * DataNodeEntityData provides access to DataNode entity defined in
  * the database.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/entity_access/src/HierarchicalAlarmViewEntityData.h"
#include "core/exceptions/src/DataException.h"

namespace TA_Base_Core
{

    const std::string HierarchicalAlarmViewEntityData::ENTITY_TYPE          = "HierarchicalAlarmViewer";
	const std::string HierarchicalAlarmViewEntityData::COLUMN_SIZE          = "NumberOfColumn";
	const std::string HierarchicalAlarmViewEntityData::COLUMN_NAME1         = "ColumnName1";
	const std::string HierarchicalAlarmViewEntityData::COLUMN_NAME2         = "ColumnName2";
	const std::string HierarchicalAlarmViewEntityData::COLUMN_NAME3         = "ColumnName3";
	const std::string HierarchicalAlarmViewEntityData::NUMBER_OF_ROOTS      = "NumberOfRoot";
	const std::string HierarchicalAlarmViewEntityData::ROOT_NODE_PREFIX     = "RootNode"; 
	const std::string HierarchicalAlarmViewEntityData::IMPLEMENTER_PREFIX   = "Implementer"; 
	const std::string HierarchicalAlarmViewEntityData::ENDPOINT_PREFIX      = "EndPoint";
    const std::string HierarchicalAlarmViewEntityData::TRAIN_IMPLEMENTER    = "Train";
    const std::string HierarchicalAlarmViewEntityData::RADIO_IMPLEMENTER    = "Radio";

	HierarchicalAlarmViewEntityData::HierarchicalAlarmViewEntityData() 	:  
		EntityData(),
		m_isValidData ( false ),
		m_columnSize ( 0 )	
    {
    }

   HierarchicalAlarmViewEntityData::HierarchicalAlarmViewEntityData( unsigned long key ) 	:
	EntityData( key, getStaticType() ),
	m_isValidData ( false ),
	m_columnSize ( 0 )    
	{
    }

    HierarchicalAlarmViewEntityData::~HierarchicalAlarmViewEntityData()
    {
		m_columnNames.clear();
		m_rootsNImplementersMap.clear();
		m_rootsNEndPointMap.clear();
		m_rootNodes.clear();
    }

	std::string HierarchicalAlarmViewEntityData::getType()
	{
        return getStaticType();
	}

	std::string HierarchicalAlarmViewEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

    std::string HierarchicalAlarmViewEntityData::getTrainImplementer()
    {
        return TRAIN_IMPLEMENTER;
    }

	std::string HierarchicalAlarmViewEntityData::getRadioImplementer()
    {
        return RADIO_IMPLEMENTER;
    }

    IEntityData* HierarchicalAlarmViewEntityData::clone ( unsigned long key )
    {
        return new HierarchicalAlarmViewEntityData( key );
    }

    void HierarchicalAlarmViewEntityData::invalidate()
    {
        getHelper()->invalidate();
        m_isValidData = false;
    }

    void HierarchicalAlarmViewEntityData::reload()
    {
		TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
        
		// Reset all parameters back to their default values
		m_columnNames.clear();
		m_rootsNImplementersMap.clear();
		m_rootsNEndPointMap.clear();
		m_rootNodes.clear();
		m_columnSize = 0;
		m_numberOfRoots = 0;
		
        int nIndex = 0;
        std::string index;
        std::string value;
        std::map<int, std::string> rootNodes;
        std::map<int, std::string> implementers;
        std::map<int, std::string> endPoints;

        // Retrieve all parameters from the database
		EntityHelper::EntityParameterNameValueMap parameters = getHelper()->getAllParameters();

        for ( EntityHelper::EntityParameterNameValueMap::iterator itParameter = parameters.begin();
              itParameter != parameters.end();
              ++itParameter )
        {
            // get the parameter name that was retrieved
            const std::string& parameterName = itParameter->first;
            const std::string& parameterValue = itParameter->second;

			if (COLUMN_SIZE == parameterName)
			{
                m_columnSize = EntityHelper::getIntegerData( parameterValue );
			}
			else if ((COLUMN_NAME1 == parameterName) || (COLUMN_NAME2 == parameterName) || 
				(COLUMN_NAME3 == parameterName))
			{
				m_columnNames.push_back( parameterValue );
			}
			else if (NUMBER_OF_ROOTS == parameterName)
			{
                m_numberOfRoots = EntityHelper::getIntegerData( parameterValue );
			}
			else if ( parameterName.find( ROOT_NODE_PREFIX ) == 0)
			{
                index = parameterName.substr( ROOT_NODE_PREFIX.length(), parameterName.length() );
                std::istringstream ssIndex(index);
                ssIndex >> nIndex;
                rootNodes[ nIndex ] = parameterValue;
			}
			else if ( parameterName.find( IMPLEMENTER_PREFIX ) == 0)
			{
                index = parameterName.substr( IMPLEMENTER_PREFIX.length(), parameterName.length() );
                std::istringstream ssIndex(index);
                ssIndex >> nIndex;
                implementers[ nIndex ] = parameterValue;
			}
			else if ( parameterName.find( ENDPOINT_PREFIX ) == 0 )
			{
                index = parameterName.substr( ENDPOINT_PREFIX.length(), parameterName.length() );
                std::istringstream ssIndex(index);
                ssIndex >> nIndex;
                endPoints[ nIndex ] = parameterValue;
			}
        }

        for ( nIndex = 1; nIndex <= m_numberOfRoots; ++nIndex )
        {
            std::string rootNode = rootNodes[ nIndex ];
            std::string implementer = implementers[ nIndex ];
            std::string endPoint = endPoints[ nIndex ];

            if ( false == rootNode.empty() &&
                 false == implementer.empty() &&
                 false == endPoint.empty() )
            {
                m_rootNodes.push_back( rootNode );
                m_rootsNImplementersMap[ rootNode ] = implementer;
                m_rootsNEndPointMap[ rootNode ] = endPoint;
            }
        }
		 // Parameters have been loaded - mark them as containing valid data.
        m_isValidData = true;
    }

	std::vector<std::string> HierarchicalAlarmViewEntityData::getRootNodes()
	{
	    TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
        if (!m_isValidData)
        {
            reload();
        }
		return m_rootNodes;

	}
	
	std::map<std::string, std::string> HierarchicalAlarmViewEntityData::getRootNameNEndPoint()
	{
	    TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
        if (!m_isValidData)
        {
            reload();
        }
		return m_rootsNEndPointMap;
	}
	
	std::map<std::string,std::string> HierarchicalAlarmViewEntityData::getRootNameNImplementer()
	{
	    TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
        if (!m_isValidData)
        {
            reload();
        }
		return m_rootsNImplementersMap;
	}
	
	std::vector<std::string> HierarchicalAlarmViewEntityData::getColumnNames()
	{
	    TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
        if (!m_isValidData)
        {
            reload();
        }
		return m_columnNames;
	}
	
	unsigned short HierarchicalAlarmViewEntityData::getColumnSize()
	{
	    TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
        if (!m_isValidData)
        {
            reload();
        }
		return m_columnSize;
	}
	

} //close namespace TA_Base_Core
