/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IConfigProfile.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 12:15:10 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * This is an interface for a profile from the database that can be configured and modified
  */

#if !defined(IConfigProfile_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigProfile_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{
    class IProfile;

    class IConfigProfile : public IProfile, public virtual IConfigItem
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
         * Returns the date modified for this profile.
         *
         * @return The date modified for this profile as a time_t.
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
         * getDisplayKey
         *
         * Returns the scada display key for the preferred display for this profile.
         *
         * @param int displayNumber - A profile can have several displays configured for it
         *                            This indicates which display to return a value for.
		 * @param unsigned long locationKey - the locationKey for the display (If 0, the location
		 *                                    is not required)
         *
         * @return The display key for this profile as a unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *            It will be thrown with the type NO_VALUE if there is no display for
         *            the specified display and location
         */
        virtual unsigned long getDisplayKey(unsigned long locationKey, int displayNumber) = 0;

        /**
         * getDisplayKey
         *
         * Returns the scada display key for the preferred display for this profile.
         *
         * @param std::string parameterName - A profile can have several displays configured for it
         *                            This indicates which display to return a value for.
		 * @param unsigned long locationKey - the locationKey for the display (If 0, the location
		 *                                    is not required)
         *
         * @return The display key for this profile as a unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *            It will be thrown with the type NO_VALUE if there is no display for
         *            the specified display and location
         */
        virtual std::string getParameterActualValue(unsigned long locationKey, const std::string& parameterName) = 0;

        
        /**
         * setType
         *
         * Sets the type of this profile
         *
         * @param unsigned long - The type of profile
         *
         * pre: This profile has not been deleted
         */
        virtual void setType(const unsigned long ) =0;


        /**
         * setDisplay
         *
         * This will set one of the displays for the profile.
         *
         * @param unsigned long location - This is the location key that this display is for
         * @param unsigned long screenNumber - This is the screen number the display is for. Numbering starts
         *                                     at 1.
         * @param long display - This is the key of the display from the SC_SCADA_SCHEMATIC table. If this is
         *                       a negative value then that means that no display should be set.
         */
        virtual void setDisplay(unsigned long location, unsigned long screenNumber, long display) =0;


        /**
         * setDisplay
         *
         * This will set one of the displays for the profile.
         *
         * @param unsigned long location - This is the location key that this display is for
         * @param const std::string parameterName - This is the display name representing the display.
         * @param long display - This is the key of the display from the SC_SCADA_SCHEMATIC table. If this is
         *                       a negative value then that means that no display should be set.
         */
        virtual void setParameterValue(unsigned long location, const std::string& parameterName, const std::string& parameterValue) =0;


        /**
         * removeDisplay
         *
         * This will remove a display from the profile for the specified location and screenNumber
         *
         * @param unsigned long location - This is the location key that the display is to be removed for
         * @param unsigned long screenNumber - This is the screen number the display is to be removed for.
         *                                      Numbering starts at 1.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual void removeDisplay(unsigned long location, unsigned long screenNumber) =0;


        /**
         * removeDisplay
         *
         * This will remove a display from the profile for the specified location and screenNumber
         *
         * @param unsigned long location - This is the location key that the display is to be removed for
         * @param unsigned long parameterName - This is the display name representing the display.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual void removeParameter(unsigned long location, const std::string& parameterName) =0;

        
        /**
         * deleteThisObject
         *
         * Removes this profile from the database. 
         * The calling application MUST then delete this object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - apply() has already been called
         *      This profile has not been deleted
         */
        virtual void deleteThisObject() = 0;

        /**
         * setAccessControlGroup
         *
         * Sets the access control group of this profile
         *
         * @param unsigned long - The access control group key
         *
         * pre: This profile has not been deleted
         */
        virtual void setAccessControlGroup(unsigned long accessControlGroupKey) = 0;

        /**
         * setRadioLoginGroup
         *
         * Sets the radio login group of this profile
         *
         * @param unsigned long - The radio login group key
         *
         * pre: This profile has not been deleted
         */
        virtual void setRadioLoginGroup(const std::string& radioGroupName) = 0;

        /**
         * setPaPriorityGroup
         *
         * Sets the radio login group of this profile
         *
         * @param unsigned long - The radio login group key
         *
         * pre: This profile has not been deleted
         */
        virtual void setPaPriorityGroup(const std::string& paGroupName) = 0;

        /**
         * setLocationAssociationStatus
         *
         * Sets the status of the association with the specified location
         *
         * @param locationId The ID of the location for which the assocation status is to be set
         * @param isAssociated True if the profile is associated with the specified location
         *
         * pre: This profile has not bene deleted
         */
        virtual void setLocationAssociationStatus(const unsigned long locationId, const bool isAssociated) = 0;


        virtual ~IConfigProfile() {};
    };
} //close namespace TA_Base_Core
#endif // !defined(IConfigProfile_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
