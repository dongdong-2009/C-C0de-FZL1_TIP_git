/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RadioStateSynchronisationEntityData.h $
 * @author:  John Dalin
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * 
 * RadioStateSynchronisationEntityData is an implementation of IEntityData. It proves read-only access to Radio state synchronisation data,
 * and a specific framework for Radio state synchronisation parameters.
 *
 */

#if !defined(Radio_State_Synchronisation_Entity_Data_INCLUDED_)
#define Radio_State_Synchronisation_Entity_Data_INCLUDED_

#include "core/data_access_interface/entity_access/src/EntityData.h"

#if defined(_MSC_VER)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

namespace TA_Base_Core
{
    class RadioStateSynchronisationEntityData : public EntityData
    {

    public:
        /**
         * RadioStateSynchronisationEntityData (constructor)
         *
         * Constructs a new RadioStateSynchronisationEntityData object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        RadioStateSynchronisationEntityData() {};

        /**
         * Radio (constructor)
         *
         * Constructs a new RadioStateSynchronisationEntityData object with the specified key.
         *
         * @param key The key to this RadioStateSynchronisationEntityData in the database.
         */
        RadioStateSynchronisationEntityData(unsigned long key);

        virtual ~RadioStateSynchronisationEntityData();

    public:

		//**********************
		// Entity Data Methods *
		//**********************

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
        RadioStateSynchronisationEntityData( const RadioStateSynchronisationEntityData&);           
		RadioStateSynchronisationEntityData& operator=(const RadioStateSynchronisationEntityData&);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
    };
    
    typedef boost::shared_ptr<RadioStateSynchronisationEntityData> RadioStateSynchronisationEntityDataPtr;
} //close namespace TA_Base_Core

#endif // !defined(Radio_State_Synchronisation_Entity_Data_INCLUDED_)
