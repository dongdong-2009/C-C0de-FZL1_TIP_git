#ifndef PREDEFINED_MESSAGE_LIBRARY_ACCESS_FACTORY_H_INCLUDED
#define PREDEFINED_MESSAGE_LIBRARY_ACCESS_FACTORY_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/tis_agent/src/PredefinedMessageLibraryAccessFactory.h $
  * @author Robin Ashcroft
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/21 18:27:55 $
  * Last modified by:  $Author: hongzhi.zhang $
  * 
  * PredefinedMessageLibraryAccessFactory is a singleton that is used to generate and retrieve 
  * IPredefinedMessageLibrary objects.
  */

#include <string>
#include <vector>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/tis_agent/src/IConfigPredefinedMessageLibrary.h"
#include "core/data_access_interface/src/DataTypeEnums.h"

namespace TA_Base_Core
{
   class IPredefinedMessageLibrary;    // Forward declaration.
}

namespace TA_Base_Core
{
    
    class PredefinedMessageLibraryAccessFactory
    {
    public:

        /**
         * ~TrainTimeScheduleAccessFactory
         *
         * Standard destructor.
         */

        virtual ~PredefinedMessageLibraryAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      TrainTimeScheduleAccessFactory&
         *              A reference to an instance of a TrainTimeScheduleAccessFactory object.
         */

		static PredefinedMessageLibraryAccessFactory& getInstance();


        /**
         * getPredefinedMessageLibraries
         *
         * Retrieves all messages libraries in the ti_predefined_message_library table
         *
         * @return      std::vector<IPredefinedMessageLibrary*>
         *              A vector of pointers to objects conforming to the IPredefinedMessageLibrary interface.
         *              NOTE: The caller is responsible for cleaning up this pointer.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */

		std::vector<IPredefinedMessageLibrary*> getPredefinedMessageLibraries(EDataTypes dbType = Tis_Cd);

        /**
         * getPredefinedMessageLibrary
         *
         * Retrieves the messages librariy in the ti_predefined_message_library table
         * with the specified version and type
         *
         * @return      IPredefinedMessageLibrary*
         *              A pointer to an object conforming to the IPredefinedMessageLibrary interface.
         *              NOTE: The caller is responsible for cleaning up this pointer.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */
		IPredefinedMessageLibrary* getPredefinedMessageLibrary( unsigned long version, const std::string& libraryType , EDataTypes dbType = Tis_Cd);

		//hongran++ TD17500
		// Used to get predefined msg from OCC local DB
		IPredefinedMessageLibrary* getOCCLocalPredefinedMessageLibrary( unsigned long version, const std::string& libraryType );

		// Used to delete data from OCC local DB
		void deleteOCCLocalAllMessageLibrariesOfTypeNotMatching( const std::string& libraryType, unsigned long currentVersion, unsigned long nextVersion );
		//++hongran TD17500


		IConfigPredefinedMessageLibrary* createNewPredefinedMessageLibrary( unsigned long version, const std::string& libraryType,
			std::vector<unsigned char>& libraryFileContent, EDataTypes conntype = Tis_Cd);

        /**
         * deleteAllMessageLibrariesOfTypeNotMatching
         *
         * Delete all messages libraries in ti_predefined_message_library table of type libraryType
         * not matching specified version
         *
         * @return      nothing
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         */
		void deleteAllMessageLibrariesOfTypeNotMatching( const std::string& libraryType, unsigned long currentVersion, unsigned long nextVersion, EDataTypes dbType = Tis_Cd );

		PredefinedMessage getPredefinedMessage( unsigned long version, std::string& libraryType, 
												int librarySection, unsigned short messageTag, EDataTypes dbType = Tis_Cd );
		// refresh ti related tables(ti_predefined_message_library & ti_predefined_message), throw database exception
		//void refreshTiTables(bool isOcc);

    private:
        //
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        //
        PredefinedMessageLibraryAccessFactory() {};
        PredefinedMessageLibraryAccessFactory& operator=(const PredefinedMessageLibraryAccessFactory &) {};
        PredefinedMessageLibraryAccessFactory( const PredefinedMessageLibraryAccessFactory& ) {};


        /** 
          * getPredefinedMessageLibrariesWhere
          *
          * gets the pre-defined message libraries based on some criteria
          *
          * @param whereSQL an SQL where clause.
          *
          * @return the pre-defined message libraries retrieved.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
        std::vector<IPredefinedMessageLibrary*> getPredefinedMessageLibrariesWhere(std::string whereSQL = "", EDataTypes dbType = Tis_Cd);

		std::vector<IPredefinedMessageLibrary*> getOCCLocalPredefinedMessageLibrariesWhere(std::string whereSQL = "");


		static PredefinedMessageLibraryAccessFactory* m_theClassInstance;

		//
        // Thread lock to protect singleton creation.
        //
        
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;
    };

} // end namespace TA_Base_Core

#endif
