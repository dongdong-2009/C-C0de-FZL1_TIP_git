/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigScadaDisplay.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigScadaDisplay is a concrete implementation of IConfigScadaDisplay, which is in turn an implementation
  * of IScadaDisplay. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for displays.
  */


#if !defined(ConfigScadaDisplay_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigScadaDisplay_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>
#include <set>

#include "core/data_access_interface/src/IConfigScadaDisplay.h"

namespace TA_Base_Core
{
	class IData;
    class ScadaDisplayHelper;

    class ConfigScadaDisplay : public IConfigScadaDisplay
    {

    public:
        /**
         * ConfigScadaDisplay (constructor)
         *
         * Constructs a new instance of the ConfigScadaDisplay with no key. This is used when
         * creating a *new* display - that is, one that does not yet exist in the database.
         */
        ConfigScadaDisplay();


        /**
         * ConfigScadaDisplay (constructor)
         *
         * Constructs a new instance of ConfigScadaDisplay with the specified key. This is used 
         * when creating a ConfigScadaDisplay based around an existing display in the database.
         *
         * @param key The key to this display in the database.
         */
        ConfigScadaDisplay(const unsigned long key);

		/**
         * Constructor
         *
         * Construct a ConfigScadaDisplay class based around dataset
         */
		ConfigScadaDisplay(const unsigned long row, TA_Base_Core::IData& data);

        /**
         * ConfigScadaDisplay (copy constructor)
         *
         * @param ConfigScadaDisplay& - The display to make a copy of.
         */
        ConfigScadaDisplay( const ConfigScadaDisplay& theConfigScadaDisplay);

        
        virtual ~ConfigScadaDisplay();


    public:

        /**
         * deleteThisObject
         *
         * Removes this display from the database. 
         * The calling application MUST then delete this display object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this display was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This display has not been deleted
         */
        void deleteThisObject();


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Operator object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This display has not been deleted
         */
        void applyChanges();
        

        /**
         * getKey
         *
         * Returns the key for this display.
         *
         * @return The key for this display as an unsigned long.
         *
         * pre: Either - this display was initially loaded from the database
         *      OR     - writeOperatorData() has already been called
         *      AND    - deleteThisObject() has NOT been called
         */
        unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this display. If this is the first time data for this display
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this display as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * pre: Either - this display was initially loaded from the database
         *      OR     - setName() has already been called
         *      AND    - deleteThisObject() has NOT been called
         */
        std::string getName();


        /**
         * setName
         *
         * Sets the name of this display. This should only be used by the ConfigScadaDisplay class
         *
         * @param name The name to give this display.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void setName(const std::string& name);

        
        /**
         * getPath
         *
         * Returns the path and file name of the display
         *
         * @return The path for this display as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */
        std::string getPath();


        /**
         * setPath
         *
         * Sets the path of this Scada display.
         *
         * @param string - The path for the display
         *
         * pre: This display has not been deleted
         */
        void setPath(const std::string& path);


        /**
         * getLocation
         *
         * Returns the location for this disply.
         *
         * @return The location for this display as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        unsigned long getLocation();
		

        /**
         * getLocationName
         *
         * Returns the location name for this display.
         *
         * @return The location name for this display as a string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        std::string getLocationName();

        
        /**
         * setLocation
         *
         * Sets the location of this Scada display.
         *
         * @param unsigned long - The key of the location for this display
         *
         * pre: This display has not been deleted
         */
        void setLocation(unsigned long location);
		

    
        /**
         * getDateCreated
         *
         * Returns the date created for this display
         *
         * @return The date created for this display as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this display.
         *
         * @return The date modified for this display as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        time_t getDateModified();
		

        /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this display. We cannot use keys for identification
         * in the configuration editor because new displays do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this display
         */
        virtual unsigned long getUniqueIdentifier()
        {
            return m_uniqueIdentifier;
        };

        
        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the display. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all display changes
         */
        virtual ItemChanges getAllItemChanges()
        {
            return m_displayChanges;
        }

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the display has been changed by the user.
         *
         * @return bool - This will return true if the display does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the display changes is empty then nothing has changed
            return !m_displayChanges.empty();
        };


        /**
         * isNew
         *
         * This should be called to determine if this display is new. A new display is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the display has not been applied to the database.
         */
        virtual bool isNew();


        /**
         * invalidate
         *
         * Make the data contained by this display as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();


        private:
            
	    /**
         * updateChanges
         *
         * This updates the map recording the changes to the display. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 

        // Assignment display not used so it is made private
		ConfigScadaDisplay& operator=(const ConfigScadaDisplay&);


        // These are the names of the fixed fields that will be used in places where we are creating
        // strings that will be shown to the user.
        static const std::string NAME;
        static const std::string PATH;
        static const std::string LOCATION;


        ScadaDisplayHelper* m_displayHelper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of displays in the
                                          // Configuration Editor because new displays do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

        ItemChanges m_displayChanges; // This is a map of all changes made to the display so that they are
                                       // recorded and can be sent in audit or online update messages.
    };
} // closes TA_Base_Core

#endif // !defined(ConfigScadaDisplay_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
