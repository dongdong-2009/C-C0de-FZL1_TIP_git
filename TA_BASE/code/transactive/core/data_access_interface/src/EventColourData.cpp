/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/EventColourData.cpp $
 * @author:  Derrick Liew
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * EventColourData is an implementation of IEventColourData. It holds the data specific to a Event
 * colour entry in the database.
 */

// EventColourData.cpp: implementation of the EventColourData class.
//
//////////////////////////////////////////////////////////////////////
#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/data_access_interface/src/EventColourData.h"
#include "core/data_access_interface/src/EventColourHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	/**
	 * Constructor
	 *
	 * Construct a event colour class based around a key, this will read
	 * the data from the database and throw the any DatabaseException
	 * if not succesful
	 *
	 * @param key The key of this event colour type in the database
	 */
	EventColourData::EventColourData( const unsigned long key )
		: m_helper( new EventColourHelper (key) )
	{
	}


	EventColourData::EventColourData(const unsigned long row, TA_Base_Core::IData& data)
		: m_helper( new EventColourHelper (row, data) )
	{
	}


	EventColourData::~EventColourData()
	{
        try
        {
            delete m_helper;
            m_helper = NULL;
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                "Unknown exception caught in destructor");
        }
	}

	/**
	 * getKey
	 *
	 * Returns the key for this Event Colour type.
	 *
	 * @return The key for this Event Colour as an unsigned long.
	 */
	unsigned long EventColourData::getKey()
	{
        TA_ASSERT(m_helper != NULL, "The EventColourData helper is NULL");

        return m_helper->getKey();
	}


	/**
	 * getName
	 *
	 * Returns the name of this Event Colour Type. If this is the first time data for this EventColour
	 * has been requested, all the data is loaded from the database.
	 *
	 * @return The name for this Event Colour as a std::string
	 *
	 * @exception DatabaseException A DatabaseException will be thrown if there is a
	 *            problem establishing a connection with the database, or if an
	 *            error is encountered while retrieving data.
	 * @exception DataException A DataException will be thrown if the data cannot be 
	 *            converted to the required format (e.g. persist_message should be 
	 *            either 0 or 1), or if the wrong amount of data is retrieved.
	 */
	std::string EventColourData::getName()
	{
        TA_ASSERT(m_helper != NULL, "The EventColourData helper is NULL");

        return m_helper->getName();
	}


	/**
	 * getEventColourKey
	 *
	 * Returns the severity RGB level of this event type. 
	 *
	 * @param EEventColourType - Which colour to retrieve. This defaults to OPEN_ACKED which was the
	 *                      behaviour when only one event colour was available.
	 *
	 * @return The severity RGB level of the event type as an unsigned int. 
	 *
	 * @exception DatabaseException A DatabaseException will be thrown if there is a
	 *            problem establishing a connection with the database, or if an
	 *            error is encountered while retrieving data.
	 * @exception DataException A DataException will be thrown if the data cannot be 
	 *            converted to the required format (e.g. persist_message should be 
	 *            either 0 or 1), or if the wrong amount of data is retrieved. 
	 */
	unsigned long EventColourData::getEventColourKey(EEventColourType type /*= OPEN_ACKED_FG1*/)
	{
        TA_ASSERT(m_helper != NULL, "The EventColourData helper is NULL");

		return m_helper->getEventColourKey(type);
	}


	/**
	 * invalidate
	 *
	 * Mark the data contained by this event as invalid. The next call to get...() 
	 * following a call to invalidate() will cause all the data to be reloaded from
	 * the database.
	 */
	void EventColourData::invalidate()
	{
        FUNCTION_ENTRY("EventColourData::invalidate" );
		
		m_helper->invalidate();
		
		FUNCTION_EXIT;
	}


	/**
	 * getDateCreated
	 *
	 * Returns the date created for this event colour type.
	 *
	 * @return The date created for this event colour type as a time_t.
	 *
	 * @exception DatabaseException A DatabaseException will be thrown if there is a
	 *            problem establishing a connection with the database, or if an
	 *            error is encountered while retrieving data.
	 * @exception DataException A DataException will be thrown if the data cannot be 
	 *            converted to the required format (e.g. TYPEKEY should be an 
	 *            unsigned long), or if the wrong ammount of data is retrieved, or if the
	 *            the location key is invalid (and this is not a new location).
	 */
	time_t EventColourData::getDateCreated()
	{
        TA_ASSERT(m_helper != NULL, "The EventColourData helper is NULL");

        return m_helper->getDateCreated();
	}


	/**
	 * getDateModified
	 *
	 * Returns the date modified for this event colour type.
	 *
	 * @return The date modified for this event colour type as a time_t.
	 *
	 * @exception DatabaseException A DatabaseException will be thrown if there is a
	 *            problem establishing a connection with the database, or if an
	 *            error is encountered while retrieving data.
	 * @exception DataException A DataException will be thrown if the data cannot be 
	 *            converted to the required format (e.g. TYPEKEY should be an 
	 *            unsigned long), or if the wrong ammount of data is retrieved, or if the
	 *            the location key is invalid (and this is not a new location).
	 */
	time_t EventColourData::getDateModified()
	{
        TA_ASSERT(m_helper != NULL, "The EventColourData helper is NULL");

        return m_helper->getDateModified();
	}


	void EventColourData::reload()
	{
		// Do nothing for now
	}

} // close namespace