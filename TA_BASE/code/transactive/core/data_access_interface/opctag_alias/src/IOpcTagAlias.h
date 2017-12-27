/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/opctag_alias/src/IOpcTagAlias.h $
  * @author:  Wu Mintao
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IOpcTagAlias is an interface to an OpcTagAlias object. It allows the OpcTagAlias object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IOpcTagAlias_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IOpcTagAlias_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{

    class IOpcTagAlias : public IItem
    {

    public:
        virtual ~IOpcTagAlias() { };

	    /**
	      * getOpcTagAlias
	      * 
	      * get the OPC Alias
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getOpcTagAlias() = 0; 

        /**
	      * getAttributeName
	      * 
	      * Returns the the attribute
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getAttributeName() = 0;

        /**
	      * getEntityName
	      * 
	      * get the datapoint name
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getEntityName() = 0;

	    /**
	      * long getEntityKey
	      * 
	      * get the datapoint key
	      * 
	      * @return virtual unsigned 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual unsigned long getEntityKey() = 0;
    };
} //close namespace TA_Base_Core

#endif // !defined(IOpcTagAlias_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
