/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File$
  * @author:	Darren Sampson
  * @version:	$Revision$
  *
  * Last modification:	$DateTime$
  * Last modified by:	$Author$
  *
  * Description:
  *
  */
#include "ace/OS.h"
#include "app/scada/OpcBridgeLib/src/OpcTagManager.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

namespace TA_Base_App
{
	OpcTagManager::OpcTagManager()
	{
		if (TA_Base_Core::RunParams::getInstance().isSet("UpdateTagPeriodically"))
        {
            m_bUpdatePeriodically = true;
		}
		else
		{
			m_bUpdatePeriodically = false;
		}

		//do call this static function,do not forget
		OpcTag::initDefaultValue();
	}


	OpcTagManager::~OpcTagManager()
	{
		TA_Base_Core::ThreadGuard lock(m_opcTagMapLock);
		
		OpcTagMap::iterator itTag;
		for (itTag = m_opcStatisticalTagMap.begin(); itTag != m_opcStatisticalTagMap.end(); ++itTag)
		{
			if (itTag->second != NULL) delete itTag->second;
		}
		m_opcStatisticalTagMap.clear();

		for (itTag = m_opcTagMap.begin(); itTag != m_opcTagMap.end(); ++itTag)
		{
			if (itTag->second != NULL) delete itTag->second;
		}
		m_opcTagMap.clear();
	}


	OpcTag* OpcTagManager::getOpcTag(const std::string& tagName, _variant_t & initValue, WORD & initQuality)
	{
		OpcTag* tag = NULL;
		
		TA_Base_Core::ThreadGuard lock(m_opcTagMapLock);
		
		OpcTagMap::iterator iter = m_opcTagMap.find(tagName);
		
		if (iter != m_opcTagMap.end())
		{
			tag = iter->second;
			//may be this tag is dynamic created, so tag is not init, we need init
			tag->updateTag(initValue, initQuality);
		}
		else
		{
			tag = new OpcTag(tagName, initValue, initQuality);
			bool result = m_opcTagMap.insert(OpcTagMap::value_type(tagName, tag)).second;	
			
			TA_ASSERT(result == true, "processUnknownTag() - insertion of Opc Tag failed");
			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MarkA--added tag %s", tagName.c_str());
		}
		
		return tag;
	}

	OpcTag* OpcTagManager::getOpcTag(const std::string& tagName)
	{
		OpcTag* tag = NULL;	
		TA_Base_Core::ThreadGuard lock(m_opcTagMapLock);
		
		OpcTagMap::iterator iter = m_opcTagMap.find(tagName);
		
		if (iter != m_opcTagMap.end())
		{
			tag = iter->second;
		}
		else
		{
			tag = new OpcTag(tagName);
			m_opcTagMap.insert(OpcTagMap::value_type(tagName, tag));	
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MarkB--added tag %s", tagName.c_str());
		}
				
		return tag;
	}


	OpcTag* OpcTagManager::findTag(const std::string& tagName)
	{
		OpcTag* retTag = NULL;
		TA_Base_Core::ThreadGuard lock(m_opcTagMapLock);

		OpcTagMap::iterator iter = m_opcTagMap.find(tagName);

		if (iter != m_opcTagMap.end())
		{
			retTag = iter->second;
		}

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "could not find tag (%s) in m_opcTagMap", tagName.c_str());

		return retTag;
	}

	void OpcTagManager::deleteOpcTag(const std::string& tagName)
	{
		TA_Base_Core::ThreadGuard lock(m_opcTagMapLock);
		OpcTagMap::iterator iter = m_opcTagMap.find(tagName);
		if (iter != m_opcTagMap.end())
		{
			OpcTag* tag = iter->second;
			m_opcTagMap.erase(iter);
			if (tag != NULL)
			{
				delete tag;
			}
		}
	}


    std::string OpcTagManager::getTagLocation ( const std::string & tagName ) const
	{
		std::string location = "";

		//
		// if tagName = "A.B.C.D.E.anything" then location = "A"
		//
		std::string::size_type pos = tagName.find_first_of( ".", 0 );
		if ( pos > 0 )
		{
			// remove the text after the first "." in the specified tag name
			location = tagName.substr ( 0, pos );
		}

        if (location.empty())
        {
		    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "location is empty for tag: (%s) pos = %lu", tagName.c_str(), pos);
        }

		return location;
	}


    std::string OpcTagManager::getTagObjectName ( const std::string & tagName ) const
	{
		std::string objectName = "";
		//
		// if tagName = "A.B.C.D.E.anything" then objectName = "A.B.C.D.E", attribute = "anything"
		//

		// find the last character "." in the tag name, starting at the end of the tag name
		std::string::size_type pos = tagName.find_last_of ( ".", tagName.length() );

		if ( pos > 0 )
		{
			// remove the name after the last "." in the specified tag name
			objectName = tagName.substr ( 0, pos );
		}

        if (objectName.empty())
        {
		    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "objectName is empty for tag: (%s) pos = %lu", tagName.c_str(), pos);
        }

		return objectName;
	}


    std::string OpcTagManager::getTagAttribute ( const std::string & tagName ) const
	{
		std::string attribute = "";
		//
		// if tagName = "A.B.C.D.E.anything" then objectName = "A.B.C.D.E", attribute = "anything"
		//

		// find the last character "." in the tag name, starting at the end of the tag name
		std::string::size_type pos = tagName.find_last_of ( ".", tagName.length() );

		if ( pos > 0 )
		{
			// get the name of the data point attribute associated with the specified tag
			attribute = tagName.substr ( pos + 1, tagName.length() );
		}

        if (attribute.empty())
        {
		    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "attribute is empty for tag: (%s) pos = %lu", tagName.c_str(), pos);
        }

		return attribute;
	}

    void OpcTagManager::createStatisticalTag ( const std::string & tagName )
    {
		TA_Base_Core::ThreadGuard lock(m_opcTagMapLock);
		OpcTag* tag = new OpcTag(tagName);
		std::pair<OpcTagMap::iterator, bool> result = 
			m_opcStatisticalTagMap.insert(OpcTagMap::value_type(tagName, tag));

		// Insert should succeed
		TA_ASSERT(result.second == true, "createStatisticalTag() - insertion of Opc Tag failed");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "added tag %s", tag->getTagName().c_str());
    }


    void OpcTagManager::updateStatisticalTag ( const std::string & tagName, _variant_t value )
    {
		TA_Base_Core::ThreadGuard lock(m_opcTagMapLock);
		OpcTagMap::iterator iter = m_opcStatisticalTagMap.find(tagName);

		if (iter != m_opcStatisticalTagMap.end())
		{
            iter->second->updateTag( value, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
        }
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Tag '%s' does not exists", tagName.c_str());
		}
    }
}
