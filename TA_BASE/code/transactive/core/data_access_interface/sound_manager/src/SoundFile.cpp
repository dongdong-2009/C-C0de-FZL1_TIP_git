/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/sound_manager/src/SoundFile.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class is an implementation of the ISoundFile 
  * interface. It provides read-only access to an 
  * entry in the so_sound_file table.
  *
  */

#ifdef _MSC_VER
	#pragma warning(disable:4786)
#endif

#include "SoundFile.h"
#include "SoundFileHelper.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

	SoundFile::SoundFile(unsigned long categoryKey, unsigned long priority)
	{
		m_helper = new SoundFileHelper(categoryKey, priority);
	}


	SoundFile::~SoundFile()
	{
		delete m_helper;
		m_helper = NULL;
	}


	unsigned long SoundFile::getCategoryKey()
	{
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        return m_helper->getCategoryKey();
	}


	unsigned long SoundFile::getPriority()
	{
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        return m_helper->getPriority();
	}


    std::string SoundFile::getDescription()
    {
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        return m_helper->getDescription();
    }


	std::string SoundFile::getSoundFile()
	{
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        return m_helper->getSoundFile();
	}


    time_t SoundFile::getDateCreated()
    {
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        return m_helper->getDateCreated();
    }


    time_t SoundFile::getDateModified()
    {
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        return m_helper->getDateModified();
    }


    unsigned long SoundFile::getKey()
    {
		// Fix TD15687 
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        return m_helper->getCategoryKey();
		// ajlaredo++ 
    }


    std::string SoundFile::getName()
    {
        // Sound files don't have a name.
        return "";
    }
		

    void SoundFile::invalidate()
    {
        TA_ASSERT(m_helper != NULL, "The SoundFileHelper pointer is null.");
        m_helper->invalidate();
    }

} // TA_Base_Core
