/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/IITSIGroup.h $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IITSIGroup is an interface to a ITSIGroup object. It allows the ITSIGroup 
  * object implementation to be changed (e.g. if necessary due to a database schema change) 
  * without changing code that uses it.
  */

#if !defined(IITSIGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IITSIGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{

    class IITSIGroup : public virtual IItem
    {

    public:
        virtual ~IITSIGroup() { };

		
        /**
         * getKey
         *
         * Returns the radio login group key for this radio login group.
         *
         * @return The radio login group key for this radio login group as an unsigned long
         */
        virtual unsigned long getKey() = 0;

        
        /**
         * getName
         *
         * Returns the radio login group name for this radio login group.
         *
         * @return The radio login group name as a string
         */
        virtual std::string getName() = 0;

        /**
         * invalidate
         *
         * Mark the data contained by this ITSIGroup as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

		/**
         * isGroupIsUsedInProfile
         *
         * Checks whether this profile group is used in profile table from the database. 
         * The calling application MUST NOT delete this object, as it will affect the
		 * constraint / relationship in the profile table.
         *
		 * @param key is the profile group primary key for check in profile table
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - apply() has already been called
         *      This profile group is to be deleted
         */
		virtual bool isGroupUsedInProfile(unsigned long key) = 0;

    };
} //close namespace TA_Base_Core

#endif // !defined(IITSIGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
