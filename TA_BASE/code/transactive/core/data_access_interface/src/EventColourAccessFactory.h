/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/EventColourAccessFactory.h $
 * @author:  Derrick Liew
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * EventColourAccessFactory is a singleton that is used to retrieve EventColour objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */


// EventColourAccessFactory.h: interface for the EventColourAccessFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTCOLOURACCESSFACTORY_H__CC67A4EA_87A5_415B_9B0D_F66A40D3EF48__INCLUDED_)
#define AFX_EVENTCOLOURACCESSFACTORY_H__CC67A4EA_87A5_415B_9B0D_F66A40D3EF48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IEventColourData.h"
#include <sstream>

namespace TA_Base_Core
{

	class EventColourAccessFactory  
	{
	public:

		virtual ~EventColourAccessFactory();

		/**
		 * getInstance
		 *
		 * Creates and returns an instance of this object.
		 *
		 * @return A reference to an instance of an EventColourAccessFactory object.
		 */
		static EventColourAccessFactory& getInstance();

		/**
		 * getEventColourByKey
		 *
		 * Returns the event colour data associated with the specified key as a pointer
		 * to an object conforming to the IEventColourData interface.
		 *
		 * @param key The database key to the event type to retrieve
		 *
		 * @return A pointer to an object conforming to the IAlarmTypeData interface.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there are
		 *            any problems in communicating with the database. The possilble 
		 *            reasons are:
		 *            1) Invalid connection string (as stored in RunParams)
		 *            2) Database is not/cannot be opened
		 *            3) Error while attempting to execute a database query
		 * @exception DataException A DataException will be thrown if there is no event colour type
		 *            matching the supplied key, or if there is more than one event colour type
		 *            matching the supplied key. 
		 */
		IEventColourData* getEventColourByKey(const unsigned long key, const bool readWrite = false);

		/**
		 * getAllEventColours
		 *
		 * Retrieves all specified severities and returns the associated data as a vector of  
		 * objects conforming to the IEventColourData interface.
		 *
		 * @return A vector of pointers to objects conforming to the IEventColourData interface.
		 * N.B.: It is the responsibility of the *client* to delete the returned IEventColourData 
		 * objects
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there are
		 *            any problems in communicating with the database. The possilble 
		 *            reasons are:
		 *            1) Invalid connection string (as stored in RunParams)
		 *            2) Database is not/cannot be opened
		 *            3) Error while attempting to execute a database query
		 * @exception DataException 
		 */
		std::vector<IEventColourData*> getAllEventColours(const bool readWrite = false);

		

	private:
        EventColourAccessFactory() {};
		EventColourAccessFactory( const EventColourAccessFactory& theEventColourAccessFactory);
		EventColourAccessFactory& operator=(const EventColourAccessFactory &);
		void getEventColourBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector< IEventColourData* >& eventColours, const bool readWrite);

        static EventColourAccessFactory* m_instance;
	};

} //close namespace

#endif // !defined(AFX_EVENTCOLOURACCESSFACTORY_H__CC67A4EA_87A5_415B_9B0D_F66A40D3EF48__INCLUDED_)
