#ifndef IPREDEFINED_MESSAGE_LIBRARY_H_INCLUDED
#define IPREDEFINED_MESSAGE_LIBRARY_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/tis_agent/src/IPredefinedMessageLibrary.h $
  * @author Robin Ashcroft
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/21 18:27:55 $
  * Last modified by:  $Author: hongzhi.zhang $
  * 
  * This interface contains the database retrieval and write methods
  * for Train Time Schedules for the Traveller Information System (TIS) Agent.
  */

#include "core/data_access_interface/tis_agent/IDL/src/TISMessageDataDefinitions.h"

#include <vector>
#include <string>
#include <map>

namespace TA_Base_Core
{
	/*
	struct STISAttributes
	{
		EDisplayMode displayMode;
		unsigned short scrollSpeed;
		unsigned short repeatInterval;
		unsigned short displayTime;
		EJustification justification;
		unsigned short lcdFontSize; //value from 0 to 8
		ELcdFontColour lcdFontColour;
		ELEDFontSize ledFontSize;
		ELEDFontColour ledFontColour;

        // default values as specified in the SICD
        // overridden in the message library
        STISAttributes()
            : displayMode(INSTANT_ON_STIS),
  		      scrollSpeed(8),
              repeatInterval(1),
		      displayTime(5),
              justification(CENTRED),
			  lcdFontSize(8),
			  lcdFontColour(WHITE),
		      ledFontSize(FONT_SIZE_SMALL),
              ledFontColour(LED_RED)
        {}
	};
	*/

	struct TTISLedAttributes
	{
		EDisplayMode displayMode;
		EScrollSpeed scrollSpeed;
        unsigned char perRun;
		int validityPeriod;
		int repeatInterval;
		int displayTime;
		ETTISLEDFontSize latinFontSize;
		EJustification justification;
		ETTISLEDIntensity intensity;

        TTISLedAttributes()
            : displayMode(SCROLL_LEFT),
  		      scrollSpeed(SCROLL_MEDIUM),
		      validityPeriod(0),
              perRun(1),
		      repeatInterval(1),
		      displayTime(5),
		      latinFontSize(TTIS_FONT_SIZE_SMALL), //CL20135
		      justification(CENTRED),
		      intensity(NORMAL)
        {}

	};

	typedef struct
	{
		unsigned short messageTag;
		int librarySection;
		std::string description;
		unsigned short priority;
		std::string message;
		// Is there only an english message in the content field?
		bool englishOnly;
        // the repeat interval to display in the gui
        unsigned short repeatInterval;
        unsigned char perRun;
	
	} PredefinedMessage;

    class IPredefinedMessageLibrary
    {
    public:

		// Define a type to store the library's messages
		typedef std::vector<PredefinedMessage*> PredefinedMessageList;

        /**
         * ~IPredefinedMessageLibrary
         *
         * Standard destructor.
         */
        virtual ~IPredefinedMessageLibrary() {};


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
        virtual unsigned long getKey() = 0;


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
		virtual unsigned long getVersion() = 0;


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
		virtual std::string getLibraryType() = 0;


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
		virtual void getLibraryFileContent( std::vector<unsigned char>& fileContent ) = 0;


       /**
         * getMessages
         *
         * Retrieves the list of messages for this library
         *
         * @return      PredefinedMessageList
         *              A vector of pre-defined message structures
         *              note that this class retains ownership of the data returned.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */
        virtual PredefinedMessageList getMessages() = 0;


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
        virtual PredefinedMessage* getMessage(int librarySection, unsigned short messageTag) = 0;


		/*
		virtual LEDAttributes getDefaultSTISLedAttributes() = 0;

		virtual LCDAttributesSTIS getDefaultSTISLcdAttributes() = 0;
		*/



        /** 
          * getDefaultTTISLedAttributes
          *
          * Get the default TTIS led attributes associated with
          * this message library.
          *
          * @return the structure holding the attribute values.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */


        virtual DisplayAttributes getDefaultSTISAttributes() = 0;

        virtual LCDAttributesSTIS getDefaultSTISLcdAttributes() = 0;

        virtual LEDAttributes getDefaultSTISLedAttributes() = 0;


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
		virtual void deleteThisLibrary() = 0;


        /**
          * invalidate
          *
          * Marks the data contained by this object as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */
        virtual void invalidate() = 0;

    };

} //end namespace TA_Base_Core

#endif 
