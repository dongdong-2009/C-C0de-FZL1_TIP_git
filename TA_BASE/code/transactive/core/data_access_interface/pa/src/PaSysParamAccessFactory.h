/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaSysParamAccessFactory.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * PaSysParamAccessFactory is a singleton that is used to retrieve PaHardwareParameter objects either from the
  * database or newly created. All PaHardwareParameter objects returned will adhere to the IPaHardwareParameter interface.
  */



#if !defined(PaHardwareParameterAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define PaHardwareParameterAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <map>


namespace TA_Base_Core
{
    namespace PAS_PARAM_NAME
    {
        const char* const DVA_LIB_VERSION = "PasDvaLibVersion";
        const char* const DVA_LIB_COUNTER_VALUE = "PasDvaLibCounterValue";
    }

	class PaSysParamAccessFactory
	{
    public:
        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an PaSysParamAccessFactory object.
         */
		static PaSysParamAccessFactory& getInstance();

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
         * getPaSysParamValue
         *
         * Retrieves the specified PaHardwareParameter and returns the associated data as an 
         * object conforming to the IPaHardwareParameter interface.
         *
         * @param   ulLocationKey
         * @param   strParameterName
         *
         * @return  A pointer to an object conforming to the IPaHardwareParameter interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaHardwareParameter object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaHardwareParameter matching the supplied key
         *          - if there is more than one PaHardwareParameter matching the supplied key. 
         *
         */
        std::string getPaSysParamValue( const unsigned long ulLocationKey, const std::string& strParameterName );

        /**
         * setPaSysParamValue
         *
         * Retrieves the specified PaHardwareParameter and returns the associated data as an 
         * object conforming to the IPaHardwareParameter interface.
         *
         * @param   ulLocationKey
         * @param   strParameterName
         *
         * @return  A pointer to an object conforming to the IPaHardwareParameter interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaHardwareParameter object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaHardwareParameter matching the supplied key
         *          - if there is more than one PaHardwareParameter matching the supplied key. 
         *
         */
        void setPaSysParamValue( const unsigned long ulLocationKey, const std::string& strParamName, const std::string& strParamValue );

        /**
         * getPaSysParamsByLocationKey
         *
         * Retrieves all PaHardwareParameters associated with a specific location as defined by the input
		 * location key parameter.  Returns the associated data as a vector of objects.		 
         *
         * @param   locationKey the key defining which location we want to fetch the PA DVA Messages from
         *                     each PA Dva Message with a locationKey matching this will be returned
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaHardwareParameter objects or set to false to return 
         *                     readonly IPaHardwareParameter objects.
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
        std::map<std::string, std::string> getPaSysParamsByLocationKey( unsigned long ulLocationKey );

	private:
        // Disable the following methods
		PaSysParamAccessFactory();
		PaSysParamAccessFactory( const PaSysParamAccessFactory& thePaHardwareParameterAccessFactory);
		PaSysParamAccessFactory& operator=(const PaSysParamAccessFactory& );

        /** 
         * ~PaSysParamAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaSysParamAccessFactory();

    private:
        static PaSysParamAccessFactory* m_instance;
	};

} // closes TA_Base_Core

#endif // !defined(PaHardwareParameterAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
