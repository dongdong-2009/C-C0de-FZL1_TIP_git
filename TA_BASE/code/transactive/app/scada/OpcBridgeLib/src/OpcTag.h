/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File$
  * @author  HoaVu
  * @version $Revision$
  *
  * Last modification : $DateTime$
  * Last modified by : $Author$
  *
  * OpcTag represents a tag that has been created in OPC Server.
  *
  */

#ifndef OPCTAG_H
#define OPCTAG_H

#include <string>
#include <comdef.h>

namespace TA_Base_App
{
	class OpcTag
	{
	public:

		/**
		 * Constructor
		 *
		 * @param 	tagName		The name of the tag to be created
		 * @param 	value		The value of the tag to be created
		 * @param 	quality		The quality of the tag to be created
		 * @param 	timestamp	The timestamp of the tag to be created
		 *
		 */
		OpcTag ( const std::string & tagName, _variant_t & initValue, WORD & initQuality);

		OpcTag ( const std::string & tagName);

		/**
		 * Destructor
		 *
		 */
		virtual ~OpcTag();

		/**
		 * getHandle
		 *
		 * Returns the handle to the OPC tag
		 *
		 */
		HANDLE getHandle() const;

		/**
		 * getTagName
		 *
		 * Returns the tagName of the OPC tag
		 *
		 */
		const std::string & getTagName() const;

		/**
		 * updateTag
		 *
		 * Update the OPC Tag with the specified value and quality
		 *
		 * @param 	value		The value to be updated to the OPC tag
		 * @param 	quality		The quality to be updated to the OPC tag
		 *
		 */
		void updateTag( VARIANT value, WORD quality );

		/**
		 * readTag
		 *
		 * Read the OPC Tag 
		 *
		 * @param 	value		The value to be read from the OPC tag
		 * @param 	quality		The quality to be read from the OPC tag
		 *
		 */
		bool readTag( VARIANT& value, WORD& quality );

		static void initDefaultValue();

		/*
		** Win-tech OPC Server Library wrappers
		**
		** Use these in preference to directly calling the functions.
		** These wrappers ensure calls to the API are not overlapped.
		*/

		/**
		  * createTag
		  *
		  * Wrapper for Win-tech OPC Server Library function "CreateTag"
		  *
		  */
		static HANDLE createTag(LPCSTR Name, VARIANT Value, WORD InitialQuality, BOOL IsWritable);
		
		/**
		  * updateTag
		  *
		  * Wrapper for Win-tech OPC Server Library function "UpdateTag"
		  *
		  */
		static BOOL updateTag(HANDLE TagHandle, VARIANT Value, WORD Quality);

		/**
		  * updateTagByName
		  *
		  * Wrapper for Win-tech OPC Server Library function "UpdateTagByName"
		  *
		  */
		static BOOL updateTagByName(LPCSTR Name, VARIANT Value, WORD Quality);

		/**
		  * updateTagByName
		  *
		  * Wrapper for Win-tech OPC Server Library function "ReadTagWithTimestamp"
		  *
		  */
		static BOOL readTagWithTimestamp(HANDLE TagHandle, VARIANT* Value, WORD* Quality, FILETIME* timestamp);

		/**
		  * suspendTagUpdates
		  *
		  * Wrapper for Win-tech OPC Server Library function "SuspendTagUpdates"
		  *
		  */
		static BOOL suspendTagUpdates(HANDLE TagHandle, BOOL OnOff);

		/**
		  * removeTag
		  *
		  * Wrapper for Win-tech OPC Server Library function "RemoveTag"
		  *
		  */
		static BOOL removeTag(HANDLE TagHandle);

	private:
		OpcTag( const OpcTag & obj );
		OpcTag & operator= ( const OpcTag & rhs );

    private:
		static VARIANT	m_defaultValue;		
        static WORD		m_defaultQuality;

		std::string m_tagName;
		HANDLE		m_tagHandle;
	};
}
#endif
