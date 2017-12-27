 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaDvaMessage.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaDvaMessage is an implementation of IPaDvaMessage. It holds the data specific to an PaDvaMessage entry
  * in the database, and allows read-only access to that data.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/data_access_interface/pa/src/PaDvaMessage.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    PaDvaMessage::PaDvaMessage(const unsigned long key)
     : m_paDvaMessageHelper ( new PaDvaMessageHelper(key))
    {}


    PaDvaMessage::PaDvaMessage(unsigned long row, TA_Base_Core::IData& data)
     : m_paDvaMessageHelper(new PaDvaMessageHelper(row, data))
    {
    }


    PaDvaMessage::~PaDvaMessage() 
	{
        delete m_paDvaMessageHelper;
        m_paDvaMessageHelper=NULL;
    }

    unsigned long PaDvaMessage::getKey()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getKey();
    }

    unsigned long PaDvaMessage::getLocationKey()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getLocationKey();
    }

    unsigned long PaDvaMessage::getId()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getId();
    }

    std::string PaDvaMessage::getLabel()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getLabel();
    }    
    
    std::string PaDvaMessage::getDescription()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getDescription();
    }    
    
    unsigned short PaDvaMessage::getType()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getType();
    }    

    
    //unsigned long PaDvaMessage::getTisMessageTag()
    //{
    //    TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
    //    return m_paDvaMessageHelper->getTisMessageTag();
    //}


    //unsigned long PaDvaMessage::getTisLibrarySection()
    //{
    //    TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
    //    return m_paDvaMessageHelper->getTisLibrarySection();
    //}


    //unsigned long PaDvaMessage::getTisLibraryVersion()
    //{
    //    TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
    //    return m_paDvaMessageHelper->getTisLibraryVersion();
    //}

    void PaDvaMessage::invalidate()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        m_paDvaMessageHelper->invalidate();
    }

    std::string PaDvaMessage::getName()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The m_paDvaMessageHelper pointer is null.");

        std::ostringstream name;
		name << "PA DVA Message " << m_paDvaMessageHelper->getKey();

        return name.str();
    }


} // closes TA_Base_Core
