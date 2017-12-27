/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/GlobalParameter.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * GlobalParameter is an implementation of IGlobalParameter. It holds the data specific to an GlobalParameter entry
  * in the database, and allows read-only access to that data.
  *
  */

#if !defined(GlobalParameter_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define GlobalParameter_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>
#include <vector>
#include <ctime>

#include "core/data_access_interface/src/IGlobalParameter.h"

namespace TA_Base_Core
{
	class IData;
    class GlobalParameterHelper;


    class GlobalParameter : public IGlobalParameter
    {

    public:

        /**
         * Constructor
         *
         * Construct a GlobalParameter class based around a key, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not succesful
         *
         * @param key The key of this GlobalParameter in the database
         */
        GlobalParameter( const unsigned long key );

		/**
		 * Constructor
		 *
		 * Construct a GlobalParameter class based around dataset
		 */
		GlobalParameter(const unsigned long row, TA_Base_Core::IData& data);


        /**
         * Destructor
         */
        virtual ~GlobalParameter();

        /**
         * getKey
         *
         * Returns the key for this GlobalParameter.
         *
         * @return The key for this GlobalParameter as an unsigned long.
         */
        virtual unsigned long getKey();

        /**
         * getName
         *
         * Returns the name of this GlobalParameter. If this is the first time data for this GlobalParameter
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this GlobalParameter as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();		

      
        virtual std::string getValue();


        virtual unsigned long getSubsystemKey();

        /**
         * invalidate
         *
         * Make the data contained by this GlobalParameter as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    private:
		
        // Assignment operator not used so it is made private
		GlobalParameter& operator=(const GlobalParameter&);

        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the GlobalParameterHelper
        // has only been written for ConfigGlobalParameter objects and will not copy the GlobalParameter helper
        // for a read-only object (ie this one).
        GlobalParameter( const GlobalParameter& theGlobalParameter);            	
		

        GlobalParameterHelper* m_GlobalParameterHelper;
    };
} // closes TA_Base_Core

#endif // !defined(GlobalParameter_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
