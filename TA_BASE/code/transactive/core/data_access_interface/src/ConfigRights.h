/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigRights.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigRights is an implementation of IConfigRights. It holds the data specific to a Rights entry
  * in the database, and allows read-write access to that data.
  *
  * This class does not have an invalidate() method like other similar classes as the data does not change
  * and is loaded in bulk. Because the Config Editor uses rights differently to other objects there is no need
  * to conform to the standard.
  */

#if !defined(ConfigRights_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define ConfigRights_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>
#include <ctime>

#include "core/data_access_interface/src/IConfigRights.h"

namespace TA_Base_Core
{
	class IData;
    class RightsHelper;

    class ConfigRights : public IConfigRights
    {

    public:

        /**
         * Constructor
         *
         * Construct an empty ConfigRights class ready to be populated. The profile and subsystem are fixed
         * and so must be specified on construction
         *
         * @param unsigned long - The key of the profile this rights configuration is for
         * @param unsigned long - The key of the subsystem this rights configuration is for
         * @param bool - True if this right is for a physical subsystem. False otherwise
         */
        ConfigRights( const unsigned long profileKey, unsigned long subsystemKey,
				unsigned long subsystemStateKey, bool isPhysical );


        /**
         * Constructor
         *
         * Construct a Rights class based around all of its data
         *
         * @param unsigned long - The key of the profile this rights configuration is for
         * @param unsigned long - The key of the subsystem this rights configuration is for
         * @param unsigned long - The key of the subsystem state this rights configuration is for
         * @param unsigned long - The key for the configured ActionGroup
         * @param bool - True if this right is for a physical subsystem. False otherwise
         */
        ConfigRights( const unsigned long profileKey, unsigned long subsystemKey, unsigned long subsystemStateKey,
				unsigned long actionGroupKey, bool isPhysical );


		/**
         * Constructor
         *
         * Construct a ConfigRights class based around dataset
         */
		ConfigRights(const unsigned long row, TA_Base_Core::IData& data);


        /**
         * Destructor
         */
        virtual ~ConfigRights();


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all item changes
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a ActionGroup names etc cannot be found
         */
        virtual ItemChanges getAllItemChanges();

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the item has been changed by the user.
         *
         * @return bool - This will return true if the item does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the original and changed ActionGroup key are not the same then changes have occurred.
            return !(m_actionGroupKey == m_originalActionGroupKey);
        }


        /**
         * getFormattedName
         *
         * This generates a name that can be displayed to the user to let them know what this right
         * represents
         *
         * @return string - The name in the format 'Profile <profilename>, Subsystem <subsystemname>'
         */
        virtual std::string getFormattedName();

        
        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew()
        {
            return m_isNew;
        }

        
        /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this item. We cannot use keys for identification
         * in the configuration editor because new items do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this item
         */
        virtual unsigned long getUniqueIdentifier()
        {
            return m_uniqueIdentifier;
        };


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Item object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This item has not been deleted
         */
        virtual void applyChanges();


        /**
         * deleteThisRight
         *
         * Removes this right from the database. 
         * The calling application MUST then delete this rights object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this rights was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This right has not been deleted
         */
        virtual void deleteThisRight();


        /**
         * getProfileKey
         *
         * Returns the profilekey for this rights configuration.
         *
         * @return The profile key for these rights as an unsigned long
         */
        virtual unsigned long getProfileKey()
        {
            return m_profileKey;
        }

        
        /**
         * getSubsystemKey
         *
         * Returns the subsystem key for this rights configuration.
         *
         * @return The subsystem key for these rights as an unsigned long
         */
        virtual unsigned long getSubsystemKey()
        {
            return m_subsystemKey;
        }

        /**
         * getSubsystemStateKey
         *
         * Returns the subsystem state key for this rights configuration.
         *
         * @return The subsystem state key for these rights as an unsigned long
         */
        virtual unsigned long getSubsystemStateKey()
        {
            return m_subsystemStateKey;
        }


        /**
         * getActionGroupKey
         *
         * Returns the ActionGroup key for this rights configuration.
         *
         * @return The ActionGroup key for these rights as an unsigned long
         */
        virtual long getActionGroupKey()
        {
            return m_actionGroupKey;
        }


        /**
         * setActionGroupKey
         *
         * Sets the ActionGroup key for this rights configuration.
         *
         * @param The ActionGroup key for these rights as an unsigned long
         */
        virtual void setActionGroupKey(long actionGroupKey)
        {
            m_actionGroupKey = actionGroupKey;
        }


        /**
         * isPhysicalSubsystem
         *
         * This determines whether this particular right is for a physical or non-physical
         * subsystem
         *
         * @return bool - True if this right is for a physical subsystem. False otherwise
         */
        virtual bool isPhysicalSubsystem()
        {
            return m_isPhysicalSubsystem;
        }


        /**
         * setSubsystemState
         *
         * Set the subsystem state for these rights.
         *
         * @param unsigned long - subsystem state key
         */
        virtual void setSubsystemState(unsigned long subsystemStateKey);

private:
	
        /**
         * modifyExistingRights
         *
         * This will update an existing rights in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the rights object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void modifyExistingRights();

        
        /**
         * addNewRights
         *
         * This will add a new rights in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Rights object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void addNewRights();


        // Assignment operator not used so it is made private
		ConfigRights& operator=(const ConfigRights&);

        // Copy constructor not used so it is made private
		ConfigRights( const ConfigRights& theRights);            	


        // These are the names of the fixed fields that will be used in places where we are creating
        // strings that will be shown to the user.
        static const std::string ACTIONGROUP;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of rightss in the
                                          // Configuration Editor because new rightss do not have keys.
                                          // Therefore we use this method for identification.

        static unsigned long s_nextAvailableIdentifier; // The next unique identifier available

        unsigned long m_profileKey;
        unsigned long m_subsystemKey;
		unsigned long m_subsystemStateKey;
        bool          m_isPhysicalSubsystem;

        long m_actionGroupKey;            // The current ActionGroup key
        long m_originalActionGroupKey;    // The original ActionGroup key as was loaded from the database

        bool m_isNew;                  // Indicates if this set of rights is new

public:
		static const long INVALID_ACTIONGROUP;

    };
} // closes TA_Base_Core

#endif // !defined(ConfigRights_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
