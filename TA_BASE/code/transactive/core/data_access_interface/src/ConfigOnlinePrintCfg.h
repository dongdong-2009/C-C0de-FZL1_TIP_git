/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigOnlinePrintCfg.h $
  * @author Nick Jardine
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/21 00:07:03 $
  * Last modified by: $Author: huirong.luo $
  * 
  * ConfigOnlinePrintCfg is a concrete implementation of IConfigOnlinePrintCfg, which is in turn an implementation
  * of IOnlinePrintCfg. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for OnlinePrintCfgs.
  */


#if !defined(ConfigOnlinePrintCfg_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigOnlinePrintCfg_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>
#include <set>

#include "core/data_access_interface/src/IConfigOnlinePrintCfg.h"


namespace TA_Base_Core
{
	class IData;
    class OnlinePrintCfgHelper;

    class ConfigOnlinePrintCfg : public IConfigOnlinePrintCfg
    {

    public:
        /**
         * ConfigOnlinePrintCfg (constructor)
         *
         * Constructs a new instance of the ConfigOnlinePrintCfg with no id. This is used when
         * creating a *new* OnlinePrintCfg - that is, one that does not yet exist in the database.
         */
        ConfigOnlinePrintCfg();

        /**
         * ConfigOnlinePrintCfg (constructor)
         *
         * Constructs a new instance of ConfigOnlinePrintCfg with the specified id. This is used 
         * when creating a ConfigOnlinePrintCfg based around an existing OnlinePrintCfg in the database.
         *
         * @param key The key to this OnlinePrintCfg in the database.
         */
        ConfigOnlinePrintCfg(const unsigned long idKey);

        /**
         * ConfigOnlinePrintCfg (copy constructor)
         *
         * @param ConfigOnlinePrintCfg& - The OnlinePrintCfg to make a copy of.
         */
        ConfigOnlinePrintCfg( const ConfigOnlinePrintCfg& theConfigOnlinePrintCfg);

        /**
         * ConfigOnlinePrintCfg (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see ConfigOnlinePrintCfgHelper::ConfigOnlinePrintCfgHelper(IData*)
         *
         */
        ConfigOnlinePrintCfg(unsigned long row, TA_Base_Core::IData& data);
        
        virtual ~ConfigOnlinePrintCfg();


    public:

        /**
         * deleteThisObject
         *
         * Removes this OnlinePrintCfg from the database. 
         * The calling application MUST then delete this OnlinePrintCfg object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this OnlinePrintCfg was initially loaded from the database
         *      OR     - writeOnlinePrintCfgData() has already been called
         *      This OnlinePrintCfg has not been deleted
         */
        void deleteThisObject(bool cascade=false);

        virtual unsigned long getKey();

		virtual std::string getDefaultPrinter();
        virtual unsigned long getLocation();
        virtual std::string getLocationName();
        virtual std::string getLocationDisplayName();
        virtual void setDefaultPrinter(const std::string& pPrinter);
        virtual void setLocation(unsigned long pSubsystem);
        virtual void setLocationName(const std::string& pLocationName);        

        /**
         * getName
         *
         * Returns the name of this item. For alarm plan associations this is a name built up from the
         * entity or entity type and the alarm type.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the OnlinePrintCfg object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This OnlinePrintCfg has not been deleted
         */
        void applyChanges();
    
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
        }


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the display. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all display changes
         */
        virtual TA_Base_Core::ItemChanges getAllItemChanges()
        {
            return m_OnlinePrintCfgChanges;
        }
       
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the OnlinePrintCfg has been changed by the user.
         *
         * @return bool - This will return true if the OnlinePrintCfg does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the OnlinePrintCfg changes is empty then nothing has changed
            return !m_OnlinePrintCfgChanges.empty();
        };


        /**
         * isNew
         *
         * This should be called to determine if this OnlinePrintCfg is new. A new OnlinePrintCfg is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the OnlinePrintCfg has not been applied to the database.
         */
        virtual bool isNew();

        /**
         * setName
         *
         * No applicable for OnlinePrintCfg - will just assert if called.
         *
         * @param name The name to give this item.
         *
         */
        virtual void setName(const std::string& name);


        /**
         * invalidate
         *
         * Make the data contained by this OnlinePrintCfg as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();


        private:
            
	    /**
         * updateOnlinePrintCfgChanges
         *
         * This updates the map recording the changes to the OnlinePrintCfg. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateOnlinePrintCfgChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 

		
        // Assignment OnlinePrintCfg not used so it is made private
		ConfigOnlinePrintCfg& operator=(const ConfigOnlinePrintCfg&);


        OnlinePrintCfgHelper* m_OnlinePrintCfgHelper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of guis in the
                                          // Configuration Editor because new guis do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

        TA_Base_Core::ItemChanges m_OnlinePrintCfgChanges; // This is a map of all changes made to the OnlinePrintCfg so that they are
                                       // recorded and can be sent in audit or online update messages.

    };
} // closes TA_Core

#endif // !defined(ConfigOnlinePrintCfg_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
