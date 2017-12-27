#ifndef PREDEFINED_MESSAGE_LIBRARY_HELPER_H_INCLUDED
#define PREDEFINED_MESSAGE_LIBRARY_HELPER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/tis_agent/src/PredefinedMessageLibraryHelper.h $
  * @author Robin Ashcroft
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/21 18:27:55 $
  * Last modified by: $Author: hongzhi.zhang $
  *
  * PredefinedMessageLibraryHelper is an object that is held by PredefinedMessageLibraryData and ConfigPredefinedMessageLibraryData objects.
  * It allows reading and writing a pre-defined message library to and from the database.
  */

#pragma warning ( disable : 4786 )

#include <map>
#include <climits>

#include "core/data_access_interface/tis_agent/src/IPredefinedMessageLibrary.h"
#include "core/data_access_interface/src/DataTypeEnums.h"


namespace TA_Base_Core
{
    class PredefinedMessageLibraryHelper
    {
        public:


            /**
              * PredefinedMessageLibraryHelper
              *
              * Standard Constructor
              *
              */
            PredefinedMessageLibraryHelper( EDataTypes conntype = Tis_Cd );


            /**
              * PredefinedMessageLibraryHelper
              *
              * Constructor with initial values
              *
              * @param version  The library version
              * @param libraryType  The library type (STIS or TTIS)
              * @param libraryFileContent   The raw library file
              *
              */
            PredefinedMessageLibraryHelper( unsigned long version,
                                            const std::string& libraryType,
                                            std::vector<unsigned char>& libraryFileContent,
                                            EDataTypes conntype = Tis_Cd );

            /**
              * PredefinedMessageLibraryHelper
              *
              * This constructor creates a new PredefinedMessageLibraryHelper for the specified key.
              *
              * @param       predefinedMessageLibraryPKey
              *              The database key for the Predefined Message Library.
              */
            PredefinedMessageLibraryHelper( unsigned long predefinedMessageLibraryPKey, EDataTypes conntype = Tis_Cd );


            /**
              * PredefinedMessageLibraryHelper
              *
              * Standard copy constructor.
              */
            PredefinedMessageLibraryHelper( const PredefinedMessageLibraryHelper& thePredefinedMessageLibraryHelperHelper );


            /**
             * ~PredefinedMessageLibraryHelper
             *
             * Standard destructor.
             */
            virtual ~PredefinedMessageLibraryHelper() {};


            /**
             * getKey
             *
             * Precondition:    Either - this PredefinedMessageLibrary was initially loaded from the database
             *                  OR     - addNewMessageLibraryData() has already been called.
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
            unsigned long getKey();


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
            unsigned long getVersion();


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
            std::string getLibraryType();


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
            void getLibraryFileContent( std::vector<unsigned char>& fileContent );


            /**
             * getMessages
             *
             * Retrieves all entries (stored in the ti_train_time_schedule_entry table)
            *
             *
             * @return      std::string
             *              The name of the preset.
             *
             * @param       unsigned short presetIndex
             *              The index of the preset.
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
            IPredefinedMessageLibrary::PredefinedMessageList getMessages();


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
            PredefinedMessage* getMessage( int librarySection, unsigned short messageTag );



            DisplayAttributes getDefaultSTISAttributes();

            LCDAttributesSTIS getDefaultSTISLcdAttributes();

            LEDAttributes getDefaultSTISLedAttributes();


            /**
             * addMessage
             *
             * Adds a pre-defined message to this library.
             *
             * @param       theMessage
             *              The message to add to this library
             *
             * @exception   TA_Base_Core::DatabaseException
             *              Thrown if:
             *              1. There is some error with the database connection
             *              2. An SQL statement failed to execute for some reason.
             * @exception   DataException
             *              Thrown if the data cannot be converted to the correct format.
             */
            void addMessage( const PredefinedMessage& theMessage );


            void setDefaultSTISDisplayAttributes ( const DisplayAttributes& stisAttributes,
                                                   const LCDAttributesSTIS& stisLcdAttributes,
                                                   const LEDAttributes& stisLedAttributes );


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
            void deleteThisLibrary();


            /**
             * invalidate
             *
             * Marks the data contained by this VideoOutputGroup as invalid. The next call to get...()
             * following a call to invalidate() will cause all the data to be reloaded from
             * the database.
             *
             * Precondition:    Either - this VideoOutputGroup was initially loaded from the database
             *                  OR     - writeVideoOutputGroupData() has already been called, as it does
             *                           not make any sense to invalidate a VideoOutputGroup that has not
             *                           yet been written to the database.
             */
            void invalidate();


            virtual bool isNew() const
            {
                return m_isNew;
            };


            void addNewMessageLibraryData();


        private:
            //
            // Disable the assignment operator.
            //

            PredefinedMessageLibraryHelper& operator=( const PredefinedMessageLibraryHelper& );

            /**
             * reload()
             *
             * This method reloads the data from the database. It is called when a get... method
             * is called and the data state is not valid.
             *
             * Precondition:    Either - this PredefinedMessageLibrary was initially loaded from the database
             *                  OR     - addNewMessageLibraryData() has already been called.
             */
            void reload();


            /**
              * loadLibraryFileContent
              *
              * Loads the blob field from tha database - its slow so its only done when necessary.
              *
              * @pre reload() must have been called at some point before calling this.
              */
            void loadLibraryFileContent();


            //
            // Flag to indicate if the data needs reloading.
            //
            bool m_isValidData;

            //
            // Flag to indicate if this is a new predefined message library or an existing one
            // loaded from the database.
            //
            bool m_isNew;

            //
            // The primary key in the database
            //
            unsigned long m_key;

            //
            // The library version number
            //
            unsigned long m_version;

            //
            // The library type, TTIS or STIS
            //
            std::string m_libraryType;

            //
            // THe raw library file content
            //
            std::vector<unsigned char> m_libraryFileContent;
            bool m_libraryFileContentPopulated;

            //
            // The messages that this pre-defined message library consists of
            //
            IPredefinedMessageLibrary::PredefinedMessageList m_messageList;


            //
            // the default attributes for display requests
            // These are used to set the gui controls in the guis to the default values
            // that are set in the library.
            //
            DisplayAttributes m_stisAttributes;
            LCDAttributesSTIS m_stisLcdAttributes;
            LEDAttributes m_stisLedAttributes;

            EDataTypes m_connType;
    };

} // closes TA_Base_Core

#endif // !defined(VideoOutputGroupHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
