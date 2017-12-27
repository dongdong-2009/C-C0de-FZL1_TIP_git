#ifndef PREDEFINED_MESSAGE_LIBRARY_DATA_H_INCLUDED
#define PREDEFINED_MESSAGE_LIBRARY_DATA_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/tis_agent/src/PredefinedMessageLibraryData.h $
  * @author Robin Ashcroft
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/21 18:27:55 $
  * Last modified by:  $Author: hongzhi.zhang $
  * 
  * This class contains the database retrievals and writes for Train Time Schedules
  * and their associated schedule entries, for the Traveller Information System (TIS) Agent.
  * It implements the ITrainTimeSchedule interface.
  */

#include <map>
#include "core/data_access_interface/tis_agent/src/IPredefinedMessageLibrary.h"
#include "core/data_access_interface/tis_agent/src/PredefinedMessageLibraryData.h"
#include "core/data_access_interface/tis_agent/src/PredefinedMessageLibraryHelper.h"

namespace TA_Base_Core
{
    class PredefinedMessageLibraryData : public IPredefinedMessageLibrary
    {
    public:

        /**
         * PredefinedMessageLibraryData
         *
         * Standard constructor.
         *
         * @param pkey the key of the pre-defined message library.
         */
        PredefinedMessageLibraryData( unsigned long pkey );


        /**
         * ~PredefinedMessageLibraryData
         *
         * Standard destructor.
         */
        virtual ~PredefinedMessageLibraryData();

        /**
         * getKey
         *
         * @return      The primary key of this item in the database.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if:
         *              1. The preset index was not found.
         *              2. The data cannot be converted to the correct format.
         */
        virtual unsigned long getKey();


        /** 
          * getVersion
          *
          * Retrieves the pre-defined message libraries version
          *
          * @return The version of the pre-defined message library.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
		virtual unsigned long getVersion();


        /** 
          * getLibraryType
          *
          * Gets the type of library, TTIS or STIS.
          *
          * @return the string TTIS or the string STIS.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
		virtual std::string getLibraryType();


        /** 
          * getLibraryFileContent
          *
          * Get the raw content of the library file.
          *
          * @param fileContent  The vector of characters to copy the library file into.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
		virtual void getLibraryFileContent( std::vector<unsigned char>& fileContent );


       /**
         * getMessages
         *
         * Retrieves the list of messages for this library
         *
         * @return      PredefinedMessageList
         *              A vector of pre-defined message structures
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */
        virtual PredefinedMessageList getMessages();


        /** 
          * getMessage
          *
          * get the message for the given message section and tag.
          *
          * @param librarySection
          * @param messageTag
          *
          * @return a pointer to the matching message. NULL if not found.
          *         note that this class retains ownership of the data returned.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
        virtual PredefinedMessage* getMessage(int librarySection, unsigned short messageTag);


		virtual DisplayAttributes getDefaultSTISAttributes();



        /** 
          * deleteThisLibrary
          *
          * Deletes the pre-defined message library and all associated data
          * from the database.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
		virtual void deleteThisLibrary();


        /**
          * invalidate
          *
          * Marks the data contained by this object as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */
        virtual void invalidate();


    private:
        //
        // The copy constructor and assignment operator aren't needed so they are made
        // private here to hide them and prevent their use.
        //
        PredefinedMessageLibraryData& operator=( const PredefinedMessageLibraryData& );
        PredefinedMessageLibraryData( const PredefinedMessageLibraryData& );

        //
        // Holds all the messages contained in this predefined message library
        //
        IPredefinedMessageLibrary::PredefinedMessageList m_messages;

		PredefinedMessageLibraryHelper* m_messageLibraryHelper;

         //
        // This indicates whether we currently have valid data or not. This is
        // initialised to false and is set to true once we do a read from the
        // database.
        //
        bool m_isValidData;
    };

} //end namespace TA_Base_Core

#endif 
