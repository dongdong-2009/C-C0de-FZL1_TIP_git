#ifndef PERSISTED_STATE_H
#define PERSISTED_STATE_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/PersistedState.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Provides READ-ONLY access to the Data Recording persisted
 * state tables.
 *
 */

#include <string>
#include <map>
#include <sstream>

#include "core/data_access_interface/data_recording_dai/src/DataRecordingDAI.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "boost/lexical_cast.hpp"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Core
{
    class PersistedState
    {

    public:

        typedef boost::shared_ptr< std::map<std::string,std::string> > ValueMapPtr;

        /**
         * Constructor
         *
         * This constructor will load persisted state values upon first
         * get.. function call.
         *
         * @param   entityKey   the pkey of the entity which state has been persisted
         * @param   typeName    the name of the entity's type which state has been persisted
         * @param   columnMap   the Live Property Id <-> Column Map mapping for this entity's type
         *
         */
        PersistedState( unsigned long entityKey, const std::string& typeName, const std::map<std::string, std::string>& columnMap );
        

        /**
         * Constructor
         *
         * This constructor has already pre-loaded the state values.
         *
         * @param   entityKey   the entityKey which state has been persisted
         * @param   typeName    the name of the entity's type which state has been persisted
         * @param   columnMap   the Live Property Id <-> Column Map mapping for this entity's type
         * @param   valueMap    the state values, indexed by column name
         * @param   sourceTime  the source time of the state
         * @param   updateTime  the update time of the state
         * @param   quality     the quality of the state
         *
         */
        PersistedState( unsigned long entityKey, const std::string& typeName, const std::map<std::string, std::string>& columnMap,
                        ValueMapPtr valueMap, time_t sourceTime, time_t updateTime, int quality );


        /**
         * Destructor
         *
         */
        virtual ~PersistedState();


        /**
         * getValue
         *
         * @param   valueId     the Live Property Id of the value to be retrieved
         * @param   value       this variable will be assigned the persisted value
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) Invalid data retrieved from database
         *            2) No data found for entity
         *            3) Multiple entries found for entity
         *            4) Unknown value identifier
         *
         * Implementation must be included here rather than in the .cpp due to VC6's 
         * crap support of templates.
         *
         */
        template<typename T>
        void getValue( const std::string& valueId, T& value )
        {
            std::string columnName;
            std::string valueString;
            try
            {
                testIsValid();

                valueString = (*m_valueMap)[ valueId ];
                if ( ! valueString.empty() )
                {
                    value = boost::lexical_cast<T>( valueString );
                }
            }
            catch( const boost::bad_lexical_cast& )
            {
                std::stringstream errorMessage;
                errorMessage << "Failed to cast " << valueId << " value '" << valueString
                             << "' to desired type";
                TA_THROW( DataException( errorMessage.str().c_str(), DataException::INVALID_VALUE, columnName ) ); 
            }
        }



        /**
         * setValue
         *
         * This does NOT write the value to the database. It
         * merely sets the internal value.
         *
         * @param   valueId     the Live Property Id of the value to be retrieved
         * @param   value       this variable will be assigned the persisted value
         *
         */
        template<typename T>
        void setValue( const std::string& valueId, const T& value )
        {
            testIsValid();
            (*m_valueMap)[ valueId ] = boost::lexical_cast<std::string>( value );
        }

    
	#ifdef _MSC_VER
    	/**
     	* Boolean specific implementation of getValue()
     	*/
    	template<>
    	void getValue<bool>( const std::string& valueId, bool& value )
    	{
        	testIsValid();

        	std::string& valueString = (*m_valueMap)[ valueId ];
        	if ( ! valueString.empty() )
        	{
            	value = ( valueString == DataRecordingDAI::VALUE_TRUE );
        	}
    	}

    	/**
     	* Boolean specific implementation of setValue()
     	*/
    	template<>
    	void setValue<bool>( const std::string& valueId, const bool& value )
    	{
        	testIsValid();

        	(*m_valueMap)[ valueId ] = value ? DataRecordingDAI::VALUE_TRUE : DataRecordingDAI::VALUE_FALSE;
    	}
	#endif
            

        /**
         * getEntityKey
         *
         * @return the pkey of the entity that this PersistedState represents
         *
         */
        unsigned long getEntityKey() const;


        /**
         * getSourceTime
         *
         * @return the source time for this persisted state
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) Invalid data retrieved from database
         *            2) No data found for entity
         *            3) Multiple entries found for entity
         *
         */
        time_t getSourceTime();


        /**
         * getUpdateTime
         *
         * @return the update time for this persisted state
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) Invalid data retrieved from database
         *            2) No data found for entity
         *            3) Multiple entries found for entity
         *
         */
        time_t getUpdateTime();


        /**
         * getQuality
         *
         * @return the quality for this persisted state
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) Invalid data retrieved from database
         *            2) No data found for entity
         *            3) Multiple entries found for entity
         *
         */
        int getQuality();


        /**
         * invalidate
         *
         * Flags the state as requiring a refresh from the database, on the
         * next get... function call.
         *
         */
        void invalidate();

    private:

        /**
         * testIsValid
         *
         * If the current set of data is invalid, then it is reloaded
         * from the database.
         *
         */
        void testIsValid();


        /**
         * reload
         *
         * Reloads the state values from the database
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) Invalid data retrieved from database
         *            2) No data found for entity
         *            3) Multiple entries found for entity
         *
         */
        void reload();


        const std::map<std::string, std::string>&   m_columnMap;
        ValueMapPtr m_valueMap;
        unsigned long   m_entityKey;
        time_t  m_sourceTime;
        time_t m_updateTime;
        int m_quality;
        bool m_valid;
        std::string m_typeName;
    };



    typedef boost::shared_ptr<PersistedState> PersistedStatePtr;
}

#endif // PERSISTED_STATE_H
