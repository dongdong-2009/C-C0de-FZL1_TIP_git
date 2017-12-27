/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/opctag_alias/src/OpcTagAlias.h $
  * @author:  Wu Mintao
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * OpcTagAlias is an implementation of IOpcTagAlias. It holds the data specific to a OpcTagAlias entry
  * in the database, and allows read-only access to that data.
  *
  */

#if !defined(OpcTagAlias_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define OpcTagAlias_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>
#include <ctime>

#include "core/data_access_interface/opctag_alias/src/IOpcTagAlias.h"

namespace TA_Base_Core
{
    class OpcTagAliasHelper;


    class OpcTagAlias : public IOpcTagAlias
    {

    public:

        /**
         * Constructor
         *
         * Construct a OpcTagAlias class based around a key, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not successful
         *
         * @param key The key of this OpcTagAlias in the database
         * @param name The name of this OpcTagAlias in the database
         */
        OpcTagAlias( const unsigned long entitykey, const std::string& name );

        /**
         * Constructor
         *
         * Construct a OpcTagAlias class based around all the necessary values, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not successful
         *
         * @param key The key of this OpcTagAlias in the database
         * @param name The name of this OpcTagAlias in the database
         */
        OpcTagAlias( const unsigned long key, const unsigned long entitykey, const std::string& name , const std::string& attribute);


        /**
         * Destructor
         */
        virtual ~OpcTagAlias();


        /**
         * getKey
         *
         * Returns the key for this OpcTagAlias.
         *
         * @return The key for this OpcTagAlias as an unsigned long.
         */
        virtual unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this OpcTagAlias. If this is the first time data for this OpcTagAlias
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this OpcTagAlias as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        virtual std::string getName();		

	    /**
	      * getOpcTagAlias
	      * 
	      * get the OPC Alias
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getOpcTagAlias(); 

        /**
	      * getAttributeName
	      * 
	      * Returns the the attribute
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getAttributeName();

        /**
	      * getEntityName
	      * 
	      * get the datapoint name
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getEntityName();

	    /**
	      * long getEntityKey
	      * 
	      * get the datapoint key
	      * 
	      * @return virtual unsigned 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual unsigned long getEntityKey() ;

        /**
         * invalidate
         *
         * Make the data contained by this OpcTagAlias as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    private:
		
        // Assignment operator not used so it is made private
		OpcTagAlias& operator=(const OpcTagAlias&);

        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor for the OpcTagAliasHelper
        // has only been written for ConfigOpcTagAlias objects and will not copy the OpcTagAlias helper
        // for a read-only object (ie this one).
        OpcTagAlias( const OpcTagAlias& theOpcTagAlias);            	
		

        OpcTagAliasHelper* m_opcTagAliasHelper;
    };
} // closes TA_Base_Core

#endif // !defined(OpcTagAlias_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
