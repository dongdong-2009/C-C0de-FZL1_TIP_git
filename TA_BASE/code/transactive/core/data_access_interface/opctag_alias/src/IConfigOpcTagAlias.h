/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/opctag_alias/src/IConfigOpcTagAlias.h $
  * @author:  Wu Mintao
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IConfigOpcTagAlias is an interface to a ConfigOpcTagAlias object. It allows the OpcTagAlias object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IConfigOpcTagAlias_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IConfigOpcTagAlias_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/opctag_alias/src/IOpcTagAlias.h"

namespace TA_Base_Core
{

    class IConfigOpcTagAlias : public IOpcTagAlias, virtual public IConfigItem
    {

    public:
        virtual ~IConfigOpcTagAlias() { };
        
	    /**
	      * setOpcTagAlias
	      * 
	      * set the OPC Alias
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setOpcTagAlias(std::string& aliasname) = 0; 

	    /**
	      * setEntityName
	      * 
	      * set the entity name
	      * 
	      * @return void
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setEntityName(std::string& entityname) = 0;

	    /**
	      * setEntityKey
	      * 
	      * set the entity key
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setEntityKey(unsigned long entitykey) = 0;

	    /**
	      * setAttributeName
	      * 
	      * set the attribute
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setAttributeName(std::string& attribute) = 0; 

        /**
         * getDateCreated
         *
         * Returns the date created for this location
         *
         * @return The date created for this location as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the date modified for this location.
         *
         * @return The date modified for this location as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified() =0;


        /**
         * deleteThisOpcTagAlias
         *
         * Removes this OpcTagAlias from the database. 
         * The calling application MUST then delete this object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this OpcTagAlias was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This OpcTagAlias has not been deleted
         */
        virtual void deleteThisOpcTagAlias() =0;


    };
} //close namespace TA_Base_Core

#endif // !defined(IConfig_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
