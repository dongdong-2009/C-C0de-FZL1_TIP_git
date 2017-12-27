/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IConfigProfileGroup.h $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This is an interface for a profile group from the database that can be configured and modified
  */

// IConfigProfileGroup.h: interface for the IConfigProfileGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICONFIGPROFILEGROUP_H__42A4F3E5_0532_4A46_B739_73092959E2F0__INCLUDED_)
#define AFX_ICONFIGPROFILEGROUP_H__42A4F3E5_0532_4A46_B739_73092959E2F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/IProfileGroup.h"
#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{
    class IProfileGroup;

	class IConfigProfileGroup : public IProfileGroup, public virtual IConfigItem 
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
         * setType
         *
         * Sets the type of this profile group either ITSI or PROFILE or BOTH
         *
         * @param unsigned long - The type of profile group
         *
         * pre: This profile group has not been deleted
         */
        virtual void setType(const std::string) =0;

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

		virtual ~IConfigProfileGroup() { };
	};

}// end of namespace

#endif // !defined(AFX_ICONFIGPROFILEGROUP_H__42A4F3E5_0532_4A46_B739_73092959E2F0__INCLUDED_)
