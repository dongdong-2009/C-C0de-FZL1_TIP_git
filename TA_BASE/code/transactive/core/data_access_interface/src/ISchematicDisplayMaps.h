/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/src/ISchematicDisplayMaps.h $
* @author:  Karen Graham
* @version: $Revision: #4 $
*
* Last modification: $DateTime: 2011/11/16 18:23:22 $
* Last modified by:  $Author: hui.wang $
* 
* IReportFile is an interface to an ReportFile object. It allows the ReportFile object implementation
* to be chagned (e.g. if necessary due to a database schema change) without changing code
* that uses it.
*/

#if !defined(ISchematicDisplayMaps_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define ISchematicDisplayMaps_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>

namespace TA_Base_Core
{

	class ISchematicDisplayMaps
	{
	public:
		virtual ~ISchematicDisplayMaps() { };

	public:
		
		/**
		* getSubTypeID
		*
		* @return The SubTypeID value as int.
		*
		*/
		virtual int getSubTypeID() = 0;

		/**
		* getStartUpDisPlay
		*
		* @return The StartUpDisPlay value as an string.
		*
		*/
		virtual std::string getStartUpDisPlay() = 0;


		/**
		* getSubTypeLabel
		*
		* @return The SubTypeLabel value as an string.
		*
		*/
		virtual std::string getSubTypeLabel() = 0;

	};
} //close namespace TA_Base_Core

#endif // !defined(ISchematicDisplayMaps_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
