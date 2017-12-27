/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/opctag_alias/src/OpcTagAlias.cpp $
  * @author:  Wu Mintao
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * OpcTagAlias is an implementation of IOpcTagAlias. It holds the data specific to an OpcTagAlias entry
  * in the database, and allows read-only access to that data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#pragma warning(disable:4284)
#endif

#include "core/data_access_interface/opctag_alias/src/OpcTagAlias.h"
#include "core/data_access_interface/opctag_alias/src/OpcTagAliasHelper.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    OpcTagAlias::OpcTagAlias( const unsigned long entitykey, const std::string& name )
		: m_opcTagAliasHelper( new OpcTagAliasHelper(entitykey,name) )
    {
        FUNCTION_ENTRY("constructor with entitykey and alias");
        FUNCTION_EXIT;
    }

    OpcTagAlias::OpcTagAlias( const unsigned long key, const unsigned long entitykey, const std::string& name , const std::string& attribute)
		: m_opcTagAliasHelper( new OpcTagAliasHelper(key, entitykey,name, attribute) )
    {
        FUNCTION_ENTRY("constructor with all fields");
        FUNCTION_EXIT;
    }


    OpcTagAlias::~OpcTagAlias()
    {
        FUNCTION_ENTRY("destructor");
        if (m_opcTagAliasHelper != NULL)
        {
            delete m_opcTagAliasHelper;
            m_opcTagAliasHelper = NULL;
        }
        FUNCTION_EXIT;
    }


    unsigned long OpcTagAlias::getKey()
    {
        FUNCTION_ENTRY("getKey");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        FUNCTION_EXIT;
        return m_opcTagAliasHelper->getKey();
    }


    std::string OpcTagAlias::getName()
    {
        FUNCTION_ENTRY("getName");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        FUNCTION_EXIT;
        return m_opcTagAliasHelper->getName();
    }
    
    std::string OpcTagAlias::getOpcTagAlias()
    {
        FUNCTION_ENTRY("getOpcTagAlias");
        //same as get name here, since the name here is the OPC alias
        FUNCTION_EXIT;
        return (getName());
    }

    std::string OpcTagAlias::getAttributeName()
    {
        FUNCTION_ENTRY("getAttributeName");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        FUNCTION_EXIT;
        return m_opcTagAliasHelper->getAttributeName();
    }

    std::string OpcTagAlias::getEntityName()
    {
        FUNCTION_ENTRY("getEntityName");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        unsigned long toReturn = m_opcTagAliasHelper->getEntitykey();
        std::string entityname;
        IEntityData* data = NULL;
        try
        {
            data = EntityAccessFactory::getInstance().getEntity(toReturn);
            entityname = data->getName();
        }
        catch (DataException) 
        {
            entityname = "";
            LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                "Can not find the entity name for the entitykey = %d", toReturn);
        }
        catch (...) 
        {
            entityname = "";
            LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                "Can not find the entity name for the entitykey = %d", toReturn);
        }
        if (data != NULL )
        {
            delete data;
        }
        FUNCTION_EXIT;
        return entityname;
    }

    unsigned long OpcTagAlias::getEntityKey()
    {
        FUNCTION_ENTRY("getEntityKey");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        FUNCTION_EXIT;
        return m_opcTagAliasHelper->getEntitykey();
    }

    
    void OpcTagAlias::invalidate()
    {
        FUNCTION_ENTRY("invalidate");
        TA_ASSERT(m_opcTagAliasHelper != NULL, "The OpcTagAlias helper is NULL");
        FUNCTION_EXIT;
        m_opcTagAliasHelper->invalidate();
    }

} // closes TA_Base_Core
