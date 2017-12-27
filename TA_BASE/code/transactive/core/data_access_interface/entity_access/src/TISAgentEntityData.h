/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/TISAgentEntityData.h $
  * @author:  Robin Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * TISAgentEntityData is a concrete implementation of ITISAgentEntityData, which is in turn an implementation of
  * IEntityData. It proves read/write access to TISAgent entity data
  *
  */

#ifndef TISAGENT_ENTITY_DATA_H
#define TISAGENT_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/ITISAgentEntityData.h"

#if defined(_MSC_VER)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

namespace TA_Base_Core
{
    class TISAgentEntityData : public ITISAgentEntityData, public EntityData
    {

    public:
        /**
         * TISAgentEntityData (constructor)
         *
         * Constructs a new TISAgentEntityData object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        TISAgentEntityData() {};

        /**
         * TISAgent (constructor)
         *
         * Constructs a new TISAgent object with the specified key.
         *
         * @param key The key to this Console in the database.
         */
        TISAgentEntityData(unsigned long key);

        virtual ~TISAgentEntityData();

    public:

		/** 
          * setParameter
          *
          * Sets the value for the named bool parameter.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      const std::string& name
          *             The name of the parameter to set.
          * @param      bool
          *             The new parameter value.
          * @param      bool& parameter
          *             The stored parameter to be set.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setParameter( const std::string& name, bool value );

        /** 
          * setParameter
          *
          * Sets the value for the named string parameter.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      const std::string& name
          *             The name of the parameter to set.
          * @param      const std::string& value
          *             The new parameter value.
          * @param      std::string& parameter
          *             The stored parameter to be set.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setParameter( const std::string& name, const std::string& value );

 		/**
         * getCurrentTimeScheduleVersion
         *
         * Returns the current ISCS Train Time Schedule Version
         *
         * @return The integer value representing the version number
         */
        virtual std::string getCurrentTimeScheduleVersion();

       /**
	     * getStatusPollInterval
         *
         * Returns the period of time (in milliseconds) that the TISAgent server should
		 * wait between sending Status Request messages to the STIS server
		 *  Predefined Message Library
         *
         * @return boolean value
         */
		virtual int getStatusPollInterval();

		virtual std::string getAssetName();

        /**
         * clone
         *
         * Returns a pointer to a new Console object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the Console object.
         */
        static IEntityData* clone(unsigned long key);

        /**
         * getType
         *
         * Returns the console type of this console (i.e. "Console").
         *
         * @return The console type of this console as a std::string
         */
        virtual std::string getType();
		static std::string getStaticType();

        /**
         * invalidate
         *
         * Mark the data contained by this console as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    private:
        //
        // Copy constructor and assignment operator are not used
        //
        TISAgentEntityData( const TISAgentEntityData& theTISAgentData) {};            
		TISAgentEntityData& operator=(const TISAgentEntityData &) {};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string CURRENT_MESSAGE_LIBRARY_VERSION;
        static const std::string NEW_MESSAGE_LIBRARY_VERSION;
		static const std::string CURRENT_TIME_SCHEDULE_VERSION;
		static const std::string STATION_LIBRARIES_SYNCHRONISED;
		static const std::string STATUS_POLL_INTERVAL;

		static const std::string PARAMETER_TRUE;
		static const std::string PARAMETER_FALSE;

		static const std::string ASSET_NAME;
    };
    
    typedef boost::shared_ptr<TISAgentEntityData> TISAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif // !defined(Radio_Entity_Data_INCLUDED_)
