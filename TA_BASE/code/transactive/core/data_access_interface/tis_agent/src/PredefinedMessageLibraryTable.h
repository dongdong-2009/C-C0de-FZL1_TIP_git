#ifndef PREDEFINED_MESSAGE_LIBRARY_TABLE_H_INCLUDED
#define PREDEFINED_MESSAGE_LIBRARY_TABLE_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/tis_agent/src/PredefinedMessageLibraryTable.h $
  * @author Robin Ashcroft
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/21 18:27:55 $
  * Last modified by: $Author: hongzhi.zhang $
  * 
  * Contains the details for the PredefinedMessageLibrary table(s).
  * This facilitates the storage of the New & Current STIS and TTIS libraries
  */

#include <string>

namespace TA_Base_Core
{
    namespace TA_TISAgentDAI
    {

        const std::string LIBRARY_TYPE_STIS  = "STIS";
        const std::string LIBRARY_TYPE_TTIS  = "TTIS";

		////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//  Predefined Message Library Table
		//
		////////////////////////////////////////////////////////////////////////////////////////////////////////
        const std::string PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME      = "ti_predefined_message_library";
		const std::string PREDEFINED_MESSAGE_LIBRARY_PKEY_SEQUENCE		  = "tipmli_seq.nextval";

		const std::string PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN	= "tipmli_id";
		const std::string VERSION_COLUMN					  = "version";
		const std::string LIBRARY_TYPE_COLUMN			  = "library_type"; // STIS or TTIS
		const std::string LIBRARY_FILE_CONTENT_COLUMN     = "library_file_content";


		
 		////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//  Predefined Message Table
		//
		////////////////////////////////////////////////////////////////////////////////////////////////////////       
		const std::string PREDEFINED_MESSAGE_TABLE_NAME = "ti_predefined_message";
		const std::string PREDEFINED_MESSAGE_PKEY_SEQUENCE		  = "tipmes_seq.nextval";

		const std::string PREDEFINED_MESSAGE_PKEY_COLUMN = "tipmes_id";
		const std::string PARENT_LIBRARY_COLUMN			= "tipmli_id";
		const std::string MESSAGE_TAG_COLUMN = "message_tag";
		const std::string LIBRARY_SECTION_COLUMN = "library_section";
		const std::string DESCRIPTION_COLUMN = "description";
		const std::string PRIORITY_COLUMN = "priority";
        const std::string REPEAT_INTERVAL_COLUMN = "repeat_interval";
		const std::string MESSAGE_CONTENT_COLUMN = "message_content";
		const std::string ENGLISH_ONLY_COLUMN = "english_only";


		////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//  Default AdHoc Attributes Table
		//
		////////////////////////////////////////////////////////////////////////////////////////////////////////		
		const std::string STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE = "TI_ADHOC_DEFAULT_ATTRIBUTES";
        const std::string STIS_DEFAULT_ADHOC_ATTRIBUTES_PKEY_SEQUENCE = "TIADAT_SEQ.NEXTVAL";
		const std::string STIS_DEFAULT_ADHOC_ATTRIBUTES_PKEY_COLUMN = "TIADAT_ID";
        const std::string STIS_DEFAULT_ADHOC_ATTRIBUTES_PARENT_LIBRARY_COLUMN = "TIPMLI_ID";

		const std::string STIS_DEFAULT_DISPLAY_MODE_COLUMN = "STIS_DISPLAY_MODE";
		const std::string STIS_DEFAULT_SCROLL_SPEED_COLUMN = "STIS_SCROLL_SPEED";
		const std::string STIS_DEFAULT_REPEAT_INTERVAL_COLUMN = "STIS_REPEAT_INTERVAL";
		const std::string STIS_DEFAULT_DISPLAY_TIME_COLUMN = "STIS_DISPLAY_TIME";
		const std::string STIS_DEFAULT_JUSTIFICATION_COLUMN = "STIS_JUSTIFICATION";
		const std::string STIS_LCD_DEFAULT_FONT_SIZE_COLUMN = "STIS_LCD_FONT_SIZE";
		const std::string STIS_LCD_DEFAULT_FONT_COLOUR_COLUMN = "STIS_LCD_FONT_COLOUR";
		const std::string STIS_LED_DEFAULT_FONT_SIZE_COLUMN = "STIS_LED_FONT_SIZE";
		const std::string STIS_LED_DEFAULT_FONT_COLOUR_COLUMN = "STIS_LED_FONT_COLOUR";
    }
}

#endif 
