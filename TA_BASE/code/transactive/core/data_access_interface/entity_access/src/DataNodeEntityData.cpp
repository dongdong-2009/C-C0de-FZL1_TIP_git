/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/DataNodeEntityData.cpp $
  * @author Darren Sampson
  * @version $Revision: #4 $
  * Last modification: $DateTime: 2016/01/18 15:51:59 $
  * Last modified by: $Author: Ouyang $
  * 
  * DataNodeEntityData provides access to DataNode entity defined in
  * the database.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/exceptions/src/DataException.h"

namespace TA_Base_Core
{

    const std::string DataNodeEntityData::ENTITY_TYPE                       = "DataNode";
	const std::string DataNodeEntityData::DATANODE_LABEL = "Label";
	const std::string DataNodeEntityData::DATANODE_SHORT_LABEL = "ShortLabel";
	const std::string DataNodeEntityData::DATANODE_ASSET_NAME = "AssetName";
	const std::string DataNodeEntityData::DATANODE_DISPLAY_ORDER = "DisplayOrder";
	const std::string DataNodeEntityData::DATANODE_LIST_OF_CHILD_ENTITIES = "ListOfChildEntities";
	const std::string DataNodeEntityData::DATANODE_SYNTHESIS_TYPE = "SynthesisType";
	const std::string DataNodeEntityData::DATANODE_SYNTHESIS_TYPE_HIERARCHY = "HIERARCHY";
	const std::string DataNodeEntityData::DATANODE_SYNTHESIS_TYPE_LIST = "LIST";
	const std::string DataNodeEntityData::DATANODE_SYNTHESIS_TYPE_BOTH = "BOTH";
	const std::string DataNodeEntityData::DATANODE_SYNTHESIS_TYPE_NONE = "NO";
	//Using for IEC 104 Protocol
	const std::string DataNodeEntityData::DATANODE_PSCADA_FLAG = "PScadaFlag";


	
	DataNodeEntityData::DataNodeEntityData()
    {
	}


    DataNodeEntityData::DataNodeEntityData ( unsigned long key )
	:
	EntityData( key, getStaticType() ),
	m_isChildInit(false)
    {
		m_childDataPoints.clear();
		m_childDataNodes.clear();
	}

    DataNodeEntityData::~DataNodeEntityData()
    {
		m_childDataPoints.clear();
		m_childDataNodes.clear();
    }

	std::string DataNodeEntityData::getType()
	{
        return getStaticType();
	}

	std::string DataNodeEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

    IEntityData* DataNodeEntityData::clone ( unsigned long key )
    {
        return new DataNodeEntityData ( key );
    }

    void DataNodeEntityData::invalidate()
    {
        getHelper()->invalidate();

		{
			TA_Base_Core::ThreadGuard guard(m_childInitLock);
			m_isChildInit = false;
		}

		m_childDataPoints.clear();
		m_childDataNodes.clear();
    }

	bool DataNodeEntityData::setParameterValues ( std::map < std::string, std::string > & parameters )
	{
		bool result = false;

		// just in case if an exception (don't care what type) is thrown
		try
		{
			// update this DataNode entity with the new parameter value
			getHelper()->writeParameters ( parameters );

			// reset the valid data flag
			invalidate();

			result = true;
		}
		catch ( ... )
		{
		}

		return result;
	}


	const std::string & DataNodeEntityData::getLabel()
	{
        m_label = getStringData ( DATANODE_LABEL );
        return m_label;
	}

	bool DataNodeEntityData::setLabel ( const std::string & newLabel )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATANODE_LABEL ] = newLabel;

		// update this DataNode entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string & DataNodeEntityData::getShortLabel()
	{
        m_shortLabel = getStringData ( DATANODE_SHORT_LABEL );
        return m_shortLabel;
	}

	bool DataNodeEntityData::setShortLabel ( const std::string & newShortLabel )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATANODE_SHORT_LABEL ] = newShortLabel;

		// update this DataNode entity with the new parameter value
		return setParameterValues ( parameters );
	}

	const std::string & DataNodeEntityData::getAssetName()
	{
		m_assetName = getStringData ( DATANODE_ASSET_NAME );
        return m_assetName;
	}

	bool DataNodeEntityData::setAssetName ( const std::string & newAssetName )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATANODE_ASSET_NAME ] = newAssetName;

		// update this DataNode entity with the new parameter value
		return setParameterValues ( parameters );
	}

	int DataNodeEntityData::getDisplayOrder()
	{
		m_displayOrder = getIntegerData ( DATANODE_DISPLAY_ORDER );
       	return m_displayOrder;
	}


	bool DataNodeEntityData::setDisplayOrder ( int orderNumber )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		char temp[100] = {0};
		sprintf ( temp, "%d", orderNumber );
		parameters [ DATANODE_DISPLAY_ORDER ] = temp;

		// update this DataNode entity with the new parameter value
		return setParameterValues ( parameters );
	}


	const std::string & DataNodeEntityData::getListOfChildEntities()
	{
		m_listOfChildEntities = getStringData ( DATANODE_LIST_OF_CHILD_ENTITIES );
       	return m_listOfChildEntities;
	}


    std::vector < std::string > DataNodeEntityData::getParsedListOfChildEntities()
    {
        std::vector< std::string > childEntities;
		parseTheListForEntityName(getListOfChildEntities(), childEntities);
        return childEntities;
    }


	bool DataNodeEntityData::setListOfChildEntities ( const std::string & newListOfChildEntities )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATANODE_LIST_OF_CHILD_ENTITIES ] = newListOfChildEntities;

		// update this DataNode entity with the new parameter value
		return setParameterValues ( parameters );
	}


	const std::string & DataNodeEntityData::getSynthesisType()
	{
		m_synthesisType = getStringData ( DATANODE_SYNTHESIS_TYPE );
       	return m_synthesisType;
	}

    
    DataNodeEntityData::ESynthesisType DataNodeEntityData::getSynthesisTypeEnum()
	{
        std::string type = getSynthesisType();

        if ( DATANODE_SYNTHESIS_TYPE_LIST == type )
		{
            return List;
		}
		if ( DATANODE_SYNTHESIS_TYPE_HIERARCHY == type )
		{
            return Hierarchy;
		}
		if ( DATANODE_SYNTHESIS_TYPE_BOTH == type )
		{
            return Both;
		}
		if ( DATANODE_SYNTHESIS_TYPE_NONE == type )
		{
            return None;
		}
       	
        // default to hierarchy
        return Hierarchy;
	}


	bool DataNodeEntityData::setSynthesisType ( const std::string & newSynthesisType )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATANODE_SYNTHESIS_TYPE ] = newSynthesisType;

		// update this DataNode entity with the new parameter value
		return setParameterValues ( parameters );
	}

	std::string DataNodeEntityData::getStringData ( const std::string & parameterName )
	{
		std::string data = getHelper()->getParameter( parameterName );

		return data;
	}

	bool DataNodeEntityData::getBooleanData ( const std::string & parameterName )
	{
		bool data = false;

		try
		{
			data = EntityHelper::getBooleanData ( getHelper()->getParameter( parameterName ) );
		}
		catch( const DataException& e )
		{
			// quieten down the logs

			if (e.getFailType() == DataException::NO_VALUE)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
					"%s: failed to retrieve the value of missing parameter %s", 
					getName().c_str(), parameterName.c_str());
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
					"%s: failed to retrieve the value of parameter %s. %s", 
					getName().c_str(), parameterName.c_str(), e.what());
			}
		}
		catch ( ... )
		{
			// do nothing
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
				"%s: failed to retrieve the value of parameter %s", 
				getName().c_str(), parameterName.c_str());
		}

		return data;
	}
	int DataNodeEntityData::getIntegerData ( const std::string & parameterName )
	{
		int data = 0;

		try
		{
			const std::string& stringData =	getHelper()->getParameter( parameterName );
			
			if (stringData != "")
			{
				data = EntityHelper::getIntegerData( stringData );
			}
		}
		catch ( ... )
		{
		}
		
		return data;
	}


	void DataNodeEntityData::createSynthesisChild()
	{
		FUNCTION_ENTRY("DataNodeEntityData::createSynthesisChild()");

        // DataNodeAccessFactory pre-loaded data isnt available if we get here

        std::vector< std::string > childEntities = getParsedListOfChildEntities();

		SharedIEntityDataList children = EntityAccessFactory::getInstance().getEntitiesByName(childEntities);

		for(SharedIEntityDataList::iterator it = children.begin(); it != children.end(); ++it)
		{
			if((*it)->getType() == DataNodeEntityData::getStaticType())
			{
				m_childDataNodes.push_back(*it);
			}
			else if((*it)->getType() == DataPointEntityData::getStaticType())
			{
				m_childDataPoints.push_back(*it);
			}
			else
			{
				std::stringstream errorMsg;
				errorMsg << "entity " << (*it)->getName() << "is not a datapoint or datanode.";
				TA_ASSERT(false, (errorMsg.str().c_str()));
			}
		}

		FUNCTION_EXIT;
	}

	void DataNodeEntityData::createHierarchyChild()
	{
		FUNCTION_ENTRY("DataNodeEntityData::createHierarchyChild");

        // DataNodeAccessFactory pre-loaded data isnt available if we get here
		
		std::vector< IEntityData* > dpList = 
			EntityAccessFactory::getInstance().getChildEntitiesOfEntityOfType(getName(), 
			DataPointEntityData::getStaticType());
		
		std::vector< IEntityData* >::iterator it;
		for(it = dpList.begin(); it!=dpList.end(); ++it)
		{
			m_childDataPoints.push_back(boost::shared_ptr<IEntityData>(*it));
		}

		std::vector< IEntityData* > dnList = 
			EntityAccessFactory::getInstance().getChildEntitiesOfEntityOfType(getName(), 
			DataNodeEntityData::getStaticType());
		for(it = dnList.begin(); it!=dnList.end(); ++it)
		{
			m_childDataNodes.push_back(boost::shared_ptr<IEntityData>(*it));
		}

		FUNCTION_EXIT;
	}


	SharedIEntityDataList DataNodeEntityData::getChildDataPoints()
	{
		FUNCTION_ENTRY("DataNodeEntityData::getChildDataPoint");

		if (!m_isChildInit)
		{
			initChildEntity();
		}
		
		FUNCTION_EXIT;
		return m_childDataPoints;
	}

	SharedIEntityDataList DataNodeEntityData::getChildDataNodes()
	{
		FUNCTION_ENTRY("DataNodeEntityData::getChildDataNode");			
		if (!m_isChildInit)
		{
			initChildEntity();
		}
		
		FUNCTION_EXIT;
		return m_childDataNodes;
	}	

	void DataNodeEntityData::initChildEntity()
	{
		FUNCTION_ENTRY("DataNodeEntityData::initChildEntity");
		
		if( !m_isChildInit )
		{
			TA_Base_Core::ThreadGuard guard(m_childInitLock);

			if ( !m_isChildInit )
			{
				// use the DataNodeAccessFactory if available

				if( DataNodeAccessFactory::getInstance()->isDataNodeHierarchyMapInitForAgent(getAgent()) )
				{
					m_childDataPoints = DataNodeAccessFactory::getInstance()->getChildDataPoints(getKey());
					m_childDataNodes = DataNodeAccessFactory::getInstance()->getChildDataNodes(getKey());
				}
				else // no preloaded data
				{
					ESynthesisType synType = getSynthesisTypeEnum();
					switch (synType)
					{
					case DataNodeEntityData::Hierarchy:
						createHierarchyChild();
						break;
					case DataNodeEntityData::List:
						createSynthesisChild();
						break;
					case DataNodeEntityData::Both:
						createHierarchyChild();
						createSynthesisChild();
						break;
					case DataNodeEntityData::None:
					default:
						// default to "HIERARCHY"
						createHierarchyChild();
						break;
					}
				}
				m_isChildInit =true;
			}
		}

		FUNCTION_EXIT;
	}

	void DataNodeEntityData::parseTheListForEntityName( const std::string & listOfChildEntities, std::vector < std::string > & namesOfChildEntities )
    {
        std::ostringstream fnName;
        fnName << "parseTheListForEntityName() - " << getName();
        FUNCTION_ENTRY( fnName.str().c_str() );
		
        // the agreed format of the string representing the list of child entities is:
        // {entity_1_name} + {entity_2_name} + ... + {entity_n_name}
		
        std::string tempString = listOfChildEntities;
        while ( true )
        {
            // find the first "{"
            std::string::size_type firstOpenCurlyPos = tempString.find_first_of ( "{" );
			
            // if found none
			if ( firstOpenCurlyPos == std::string::npos )
            {
                // exit loop
                break;
            }
			
            // find the first "}"
            std::string::size_type firstCloseCurlyPos = tempString.find_first_of ( "}" );
			
            // get the characters between the "{" and "}"
            namesOfChildEntities.push_back ( tempString.substr ( firstOpenCurlyPos + 1, firstCloseCurlyPos - ( firstOpenCurlyPos + 1 ) ) );
			
            // move on to next {} pair
            tempString = tempString.substr ( firstCloseCurlyPos + 1 );
        }
		
        FUNCTION_EXIT;
    }

	bool DataNodeEntityData::getPScadaFlag()
	{
		return getBooleanData ( DATANODE_PSCADA_FLAG );
	}

	bool DataNodeEntityData::setPScadaFlag ( bool newPScadaFlag )
	{
		// set up the new parameter value to be written
		std::map < std::string, std::string > parameters;
		parameters [ DATANODE_PSCADA_FLAG ] = "0";

		if ( true == newPScadaFlag )
		{
			parameters [ DATANODE_PSCADA_FLAG ] = "1";
		}

		// update this DataPoint entity with the new parameter value
		return setParameterValues ( parameters );
	}
} //close namespace TA_Base_Core
