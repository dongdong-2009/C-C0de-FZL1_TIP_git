/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/LocationLimitedEntityFactory.cpp $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Provides customisation of entity factory to retrieve the SCADA tree 
  * for a particular Location, and only those tree items (datanodes or 
  * datapoints) that are displayable  (ie. they have a display order).
  *
  */

#pragma warning ( disable : 4250 4786 4284)

#include "stdafx.h"
#include "LocationLimitedEntityFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/entityaccessfactory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace TA_Base_App
{

    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    LocationLimitedEntityFactory::LocationLimitedEntityFactory( const unsigned long locationId)
        : m_locationId ( locationId)
    {
    }

    LocationLimitedEntityFactory::~LocationLimitedEntityFactory()
    {
    }


    //////////////////////////////////////////////////////////////////////
    // Interface implementations
    //////////////////////////////////////////////////////////////////////

    std::vector<TA_Base_Core::IEntityData*> LocationLimitedEntityFactory::getChildEntitiesOf(unsigned long entityKey)
    {
        std::vector<TA_Base_Core::IEntityData*> result = 
            TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf(entityKey);

        std::vector<TA_Base_Core::IEntityData*>::iterator iter = result.begin();

        //  Only display the DataNode or DataPoint if its display order is >= 0
        while ( iter != result.end() )
        {
            bool doNotDisplay = false;
            TA_Base_Core::DataPointEntityData * dataPointEntityData = 0;
            dataPointEntityData = dynamic_cast < TA_Base_Core::DataPointEntityData * > (*iter);
            if ( 0 != dataPointEntityData )
            {
                doNotDisplay = ( 0 > dataPointEntityData->getDisplayOrder() );
            }
            else
            {
                TA_Base_Core::DataNodeEntityData * dataNodeEntityData = 0;
                dataNodeEntityData = dynamic_cast < TA_Base_Core::DataNodeEntityData * > ( *iter);
                if ( 0 != dataNodeEntityData )
                {
                    doNotDisplay = ( 0 > dataNodeEntityData->getDisplayOrder() );
                }
            }
            if (doNotDisplay)
            {
                iter = result.erase( iter);
                continue;   //  iter now points at next element
            }
            iter++;
        }
        return result;
    }


    bool LocationLimitedEntityFactory::hasChildren(unsigned long entityKey, bool hasChildren)
    {
        return hasChildren;
    }


    std::vector<TA_Base_Core::IEntityData*> LocationLimitedEntityFactory::getEntitiesOfType(const std::string& entityTypeName)
    {
        // The reason for having LocationLimitedEntityFactory class: limit the SCADA tree to this location,
        // so if asked for the SCADAROOT then return the (datanode) entity that is at location-level
        // in the scada hierarchy and is for this location.

        if ( "SCADAROOT" != entityTypeName)
        {
            return TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( entityTypeName, m_locationId);
        }

        std::vector<TA_Base_Core::IEntityData*> result;

        std::vector<TA_Base_Core::IEntityData*> scadaRoot = 
            TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType( entityTypeName);

        if ( scadaRoot.size() != 1)
        {
            return result;  //  an empty vector
        }

        std::vector<TA_Base_Core::IEntityData*> locations =
            TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf( scadaRoot[0]->getKey() );

        std::vector<TA_Base_Core::IEntityData*>::const_iterator iter = locations.begin();
        for ( ; iter != locations.end(); iter++)
        {
            if ( (*iter)->getLocation() == m_locationId)
            {
                result.push_back( *iter);
                break;
            }
        }

        return result;
    }


    TA_Base_Core::IEntityData* LocationLimitedEntityFactory::getEntity(unsigned long entityKey)
    {
        // Simple pass-through
        return TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityKey);
    }


    TA_Base_Core::IEntityData* LocationLimitedEntityFactory::getEntity(const std::string& entityName)
    {
        // Simple pass-through
        return TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);
    }

} // TA_Base_App
