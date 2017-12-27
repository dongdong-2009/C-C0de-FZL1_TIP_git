 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/Operator.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Operator is an implementation of IOperator. It holds the data specific to an operator entry
  * in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/src/Operator.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    Operator::Operator(const unsigned long key)
		: m_OperatorHelper( new OperatorHelper(key) )
    {
    }

	Operator::Operator(const unsigned long key, 
                 const std::string& name,bool isSystemOperator)
	{
		 m_OperatorHelper = new OperatorHelper(key, name,isSystemOperator);
	}

    Operator::Operator(const unsigned long key, 
                 const std::string& name, 
                 const std::string& description,
                 const std::string& password,
                 bool isSystemOperator,
                 time_t dateCreated,
                 time_t dateModified,
                 bool loadProfiles, /*=false*/
                 bool loadRegions /*=false*/ )
    {
        m_OperatorHelper = new OperatorHelper(key, name, description, password, isSystemOperator, 
                                              dateCreated, dateModified, loadProfiles, loadRegions);
    }

	Operator::Operator(const unsigned long row, TA_Base_Core::IData& data, std::vector<unsigned long>& profiles, std::vector<unsigned long>& regions)
		: m_OperatorHelper( new OperatorHelper(row, data, profiles, regions) )
	{
	}


    Operator::~Operator() 
	{
        delete m_OperatorHelper;
        m_OperatorHelper=NULL;
    }


    unsigned long Operator::getKey()
    {
        TA_ASSERT(m_OperatorHelper != NULL,"The OperatorHelper pointer is null.");
        return m_OperatorHelper->getKey();
    }


    std::string Operator::getName()
    {
        TA_ASSERT(m_OperatorHelper != NULL,"The OperatorHelper pointer is null.");
        return m_OperatorHelper->getName();
    }


    std::string Operator::getDescription()
    {
        TA_ASSERT(m_OperatorHelper != NULL,"The OperatorHelper pointer is null.");
        return m_OperatorHelper->getDescription();
    }


	std::string Operator::getPassword()
	{
        TA_ASSERT(m_OperatorHelper != NULL,"The OperatorHelper pointer is null.");
		return m_OperatorHelper->getPassword();
	}


	void Operator::setPassword( const std::string password )
	{
        TA_ASSERT(m_OperatorHelper != NULL,"The OperatorHelper pointer is null.");
		m_OperatorHelper->setPassword( password );
        m_OperatorHelper->writeOperatorData();
	}
	
	
    bool Operator::isSystemOperator()
	{
        TA_ASSERT(m_OperatorHelper != NULL,"The OperatorHelper pointer is null.");
		return m_OperatorHelper->isSystemOperator();
	}


    std::vector<IProfile*> Operator::getAssociatedProfiles()
	{
        TA_ASSERT(m_OperatorHelper != NULL,"The OperatorHelper pointer is null.");
		return m_OperatorHelper->getAssociatedProfiles();
	}


    std::vector<unsigned long> Operator::getAssociatedProfileKeys()
	{
        TA_ASSERT(m_OperatorHelper != NULL,"The OperatorHelper pointer is null.");
		return m_OperatorHelper->getAssociatedProfileKeys();
	}


	std::vector<unsigned long> Operator::getAssociatedRegionKeys()
	{
        TA_ASSERT(m_OperatorHelper != NULL,"The OperatorHelper pointer is null.");
		return m_OperatorHelper->getAssociatedRegionKeys();
	}

    void Operator::invalidate()
    {
        TA_ASSERT(m_OperatorHelper != NULL,"The OperatorHelper pointer is null.");
        m_OperatorHelper->invalidate();
    }

} // closes TA_Base_Core
