/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/IConfigITSIGroup.h $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This is an interface for a ITSI group from the database that can be configured and modified
  */

// IConfigITSIGroup.h: interface for the IConfigITSIGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICONFIGITSIGROUP_H__9E188D2B_5D4A_4E7B_9DE3_11402B7D1119__INCLUDED_)
#define AFX_ICONFIGITSIGROUP_H__9E188D2B_5D4A_4E7B_9DE3_11402B7D1119__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/radio/src/IITSIGroup.h"
#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{
    class IITSIGroup;

	class IConfigITSIGroup : public IITSIGroup, public IConfigItem  
	{

	public:

        /**
         * getDateCreated
         *
         * Returns the date created for this profile
         *
         * @return The date created for this profile as a time_t.
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
         * Returns the date modified for this profile group.
         *
         * @return The date modified for this profile group as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be only 
         *            either Profile or ITSI).
         */
        virtual time_t getDateModified() =0;

		/**
         * deleteThisObject
         *
         * Removes this profile group from the database. 
         * The calling application MUST then delete this object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - apply() has already been called
         *      This profile group has not been deleted
         */
        virtual void deleteThisObject() = 0;


		virtual ~IConfigITSIGroup() { };

	};
} // end of namespace

#endif // !defined(AFX_ICONFIGITSIGROUP_H__9E188D2B_5D4A_4E7B_9DE3_11402B7D1119__INCLUDED_)
