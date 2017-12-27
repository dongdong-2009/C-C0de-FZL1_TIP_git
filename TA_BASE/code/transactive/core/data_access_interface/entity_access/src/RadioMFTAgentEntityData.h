/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RadioMFTAgentEntityData.h $
 * @author:  Alexis Laredo
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * 
 * RadioMFTAgentEntityData is an implementation of IEntityData. It proves read-only access to data,
 * and a specific framework for Radio Manager/MFT agent parameters.
 *
 */

#if !defined(Radio_Profile_Entity_Data_INCLUDED_)
#define Radio_Profile_Entity_Data_INCLUDED_

#include "core/data_access_interface/entity_access/src/EntityData.h"

#if defined(_MSC_VER)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

namespace TA_Base_Core
{
    class RadioMFTAgentEntityData : public EntityData
    {

    public:
        /**
         * RadioMFTAgentEntityData (constructor)
         *
         * Constructs a new RadioProfile object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        RadioMFTAgentEntityData() {};

        /**
         * Radio (constructor)
         *
         * Constructs a new RadioProfile object with the specified key.
         *
         * @param key The key to this Radio Profile in the database.
         */
        RadioMFTAgentEntityData(unsigned long key);

        virtual ~RadioMFTAgentEntityData();

    public:

		//*****************
		// Radio MFTAgent Specific Methods
		//*****************

        /**
         * getBIMResource
         *
         * Returns the RID of the BIM resource for this console operator 
         *
         * @return  resource id  
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */ 
        unsigned int getBIMResource();

        /**
         * getSPIResource
         *
         * Returns RID of the SPI resource for this console operator 
         *
         * @return resource id
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */
        unsigned int getSPIResource();

		unsigned int getPassNumber();
		unsigned int getClientSSI();
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

    protected:

        /**
         * reload
         *
         * (Re)loads the parameter data from the database.
         *
         * @exception DatabaseException If there is a database error.
         * @exception DataException If a parameter value cannot be converted to the required
         *            type
         */
        void reload();

    private:
        //
        // Copy constructor and assignment operator are not used
        //
        RadioMFTAgentEntityData( const RadioMFTAgentEntityData& theRadioSession) {};            
		RadioMFTAgentEntityData& operator=(const RadioMFTAgentEntityData &) {};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;

	public:
		
		static const std::string BIM_RESOURCE_ID;
		static const std::string SPI_RESOURCE_ID;
		static const std::string PASSNUMBER;
		static const std::string CLIENT_SSI;

    };	
    
    typedef boost::shared_ptr<RadioMFTAgentEntityData> RadioMFTAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif // !defined(Radio_Profile_Entity_Data_INCLUDED_)
