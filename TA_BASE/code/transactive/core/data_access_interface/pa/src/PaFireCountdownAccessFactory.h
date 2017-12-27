/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaFireCountdownAccessFactory.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * PaFireCountdownAccessFactory is a singleton that is used to retrieve PaHardwareParameter objects either from the
  * database or newly created. All PaHardwareParameter objects returned will adhere to the IPaHardwareParameter interface.
  */



#if !defined(PAHARDWAREPARAMETERACCESSFACTORY__INCLUDED_)
#define PAHARDWAREPARAMETERACCESSFACTORY__INCLUDED_

#include <string>
#include <map>


namespace TA_Base_Core
{

	class PaFireCountdownAccessFactory
	{
    public:
        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an PaFireCountdownAccessFactory object.
         */
		static PaFireCountdownAccessFactory& getInstance();

        /**
         * removeInstance
	     *
	     * Removes the instance of the class (if already created) and cleans up the members.  
         * Primarily used upon program termination (e.g. from main()) so that Purify does not 
         * consider this class and its members to be memory leaks.
         *
         */
        static void removeInstance();

        /**
         * getStnPaFireTimers
         *
         * Retrieves all PaHardwareParameters associated with a specific location as defined by the input
		 * location key parameter.  Returns the associated data as a vector of objects.		 
         *
         * @param   locationKey the key defining which location we want to fetch the PA Fire timer from
         *                     PA_FIRE_COUNTDOWN with a locationKey matching this will be returned
         *
         * @return  a vector of pointers to objects conforming to the IPaHardwareParameter interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaHardwareParameter object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        std::map<std::string, unsigned short> getStnPaFireTimers( unsigned long ulLocationKey );

        /**
         * getOccPaFireTimers
         *
         * Retrieves all PaHardwareParameters associated with a specific location as defined by the input
		 * location key parameter.  Returns the associated data as a vector of objects.		 
         *
         * @return  a vector of pointers to objects conforming to the IPaHardwareParameter interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaHardwareParameter object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        std::map<std::string, unsigned short> getOccPaFireTimers();

        /**
        * getStnPaFireAborts
        *
        * Retrieves all PaHardwareParameters associated with a specific location as defined by the input
        * location key parameter.  Returns the associated data as a vector of objects.		 
        *
        * @param   locationKey the key defining which location we want to fetch the PA Fire timer from
        *                     PA_FIRE_COUNTDOWN with a locationKey matching this will be returned
        *
        * @return  a vector of pointers to objects conforming to the IPaHardwareParameter interface.
        *          Note: It is the responsibility of the *client* to delete the 
        *          returned IPaHardwareParameter object
        *
        * @throws  DatabaseException 
        *          - if there are any problems in communicating with the database.
        *            The possible reasons are:
        *            1) Invalid connection string (as stored in RunParams)
        *            2) Database is not/cannot be opened
        *            3) Error while attempting to execute a database query
        *
        */
        std::map<std::string, unsigned short> getStnPaFireAborts( unsigned long ulLocationKey );

        /**
        * getOccPaFireAborts
        *
        * Retrieves all PaHardwareParameters associated with a specific location as defined by the input
        * location key parameter.  Returns the associated data as a vector of objects.		 
        *
        * @return  a vector of pointers to objects conforming to the IPaHardwareParameter interface.
        *          Note: It is the responsibility of the *client* to delete the 
        *          returned IPaHardwareParameter object
        *
        * @throws  DatabaseException 
        *          - if there are any problems in communicating with the database.
        *            The possible reasons are:
        *            1) Invalid connection string (as stored in RunParams)
        *            2) Database is not/cannot be opened
        *            3) Error while attempting to execute a database query
        *
        */
        std::map<std::string, unsigned short> getOccPaFireAborts();

	private:
        // Disable the following methods
		PaFireCountdownAccessFactory();
		PaFireCountdownAccessFactory( const PaFireCountdownAccessFactory& refSrc );
		const PaFireCountdownAccessFactory& operator=( const PaFireCountdownAccessFactory& refSrc );

        /** 
         * ~PaFireCountdownAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaFireCountdownAccessFactory();

    private:
        static PaFireCountdownAccessFactory* m_instance;
	};

} // closes TA_Base_Core

#endif // !defined(PAHARDWAREPARAMETERACCESSFACTORY__INCLUDED_)
