/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/src/SchematicDisplayMaps.h $
* @author:  Karen Graham
* @version: $Revision: #4 $
*
* Last modification: $DateTime: 2011/11/16 18:23:22 $
* Last modified by:  $Author: hui.wang $
* 
* ReportFile is an implementation of IReportFile. It holds the data specific to an ReportFile entry
* in the database, and allows read-only access to that data.
*/

#if !defined(SchematicDisplayMaps_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define SchematicDisplayMaps_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/src/ISchematicDisplayMaps.h"

namespace TA_Base_Core
{
	class SchematicDisplayMaps : public ISchematicDisplayMaps
	{

	public:

		/**
		* Constructor
		*
		* Construct an SchematicDisplayMaps class.
		*
		*/
		SchematicDisplayMaps(int nSubTypeID, const std::string& strStartupDisplay, const std::string& strSubTypeLable);


		/**
		* Destructor
		*/
		virtual ~SchematicDisplayMaps();

	public:

		/**
		* getSubTypeID
		*
		* @return The SubTypeID value as int.
		*
		*/
		virtual int getSubTypeID();

		/**
		* getStartUpDisPlay
		*
		* @return The StartUpDisPlay value as an string.
		*
		*/
		virtual std::string getStartUpDisPlay();


		/**
		* getSubTypeLabel
		*
		* @return The SubTypeLabel value as an string.
		*
		*/
		virtual std::string getSubTypeLabel();

	private:
		int m_nSubTypeIDValue;
		std::string m_strStartupDisplayValue;
		std::string m_strSubTypeLableValue;


	};
} // closes TA_Base_Core

#endif // !defined(SchematicDisplayMaps_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
