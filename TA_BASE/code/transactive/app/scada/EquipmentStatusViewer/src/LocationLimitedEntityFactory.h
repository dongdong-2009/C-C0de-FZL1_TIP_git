/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/LocationLimitedEntityFactory.h $
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

#if !defined(AFX_LOCATIONLIMITEDENTITYFACTORY_H__91BD3E1E_006C_4D7D_BA42_C7C8E75A193F__INCLUDED_)
#define AFX_LOCATIONLIMITEDENTITYFACTORY_H__91BD3E1E_006C_4D7D_BA42_C7C8E75A193F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #pragma once

#include "bus/scada/scada_tree/src/IEntityFactory.h"

namespace TA_Base_App
{
    class LocationLimitedEntityFactory : public TA_Base_Bus::IEntityFactory
    {
    public:
        LocationLimitedEntityFactory(const unsigned long locationId);
        virtual ~LocationLimitedEntityFactory();

        /**
        * Interface implementations (see IEntityFactory for further details)
        */
        virtual std::vector<TA_Base_Core::IEntityData*> getChildEntitiesOf(unsigned long entityKey);
        virtual std::vector<TA_Base_Core::IEntityData*> getEntitiesOfType(const std::string& entityTypeName);
        virtual TA_Base_Core::IEntityData* getEntity(unsigned long entityKey);
        virtual TA_Base_Core::IEntityData* getEntity(const std::string& entityName);
        virtual bool hasChildren(unsigned long entityKey, bool hasChildren);

    private:
        unsigned long m_locationId;

    };

} // TA_Base_App


#endif // !defined(AFX_LOCATIONLIMITEDENTITYFACTORY_H__91BD3E1E_006C_4D7D_BA42_C7C8E75A193F__INCLUDED_)
