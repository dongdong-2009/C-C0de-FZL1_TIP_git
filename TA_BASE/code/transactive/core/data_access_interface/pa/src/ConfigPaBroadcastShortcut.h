/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_T00010200/3002/transactive/core/data_access_interface/pa/src/ConfigPaBroadcastShortcut.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2010/01/11 18:41:48 $
  * Last modified by: $Author: builder $
  * 
  * ConfigPaBroadcastShortcut is a concrete implementation of IConfigPaBroadcastShortcut, which is in turn an implementation
  * of IPaDvaMessage. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaDvaMessages.
  */


#ifndef CONFIGPABROADCASTSHORTCUT__INCLUDED_
#define CONFIGPABROADCASTSHORTCUT__INCLUDED_

#include <string>
#include <set>

#include "core/data_access_interface/pa/src/IConfigPaBroadcastShortcut.h"


namespace TA_Base_Core
{

class IData;
class PaBroadcastShortcutHelper;

class ConfigPaBroadcastShortcut : public IConfigPaBroadcastShortcut
{

public:
    /**
     * ConfigPaBroadcastShortcut (constructor)
     *
     * Constructs a new instance of the ConfigPaBroadcastShortcut with no id. This is used when
     * creating a *new* PaDvaMessage - that is, one that does not yet exist in the database.
     */
    ConfigPaBroadcastShortcut();

    /**
     * ConfigPaBroadcastShortcut (constructor)
     *
     * Constructs a new instance using the information provided in IData interface
     *
     * @see PaDvaMessageHelper::PaDvaMessageHelper(IData*)
     *
     */
    ConfigPaBroadcastShortcut(unsigned long row, TA_Base_Core::IData& data);

    /**
     * ConfigPaBroadcastShortcut (constructor)
     *
     * Constructs a new instance of ConfigPaBroadcastShortcut with the specified id. This is used 
     * when creating a ConfigPaBroadcastShortcut based around an existing PaDvaMessage in the database.
     *
     * @param key The key to this PaDvaMessage in the database.
     */
    ConfigPaBroadcastShortcut(const unsigned long idKey);


    /**
     * ConfigPaBroadcastShortcut (copy constructor)
     *
     * @param ConfigPaBroadcastShortcut& - The PaDvaMessage to make a copy of.
     */
    ConfigPaBroadcastShortcut( const ConfigPaBroadcastShortcut& theConfigPaBroadcastShortcut);

    
    virtual ~ConfigPaBroadcastShortcut();


public:

    /**
     * deleteThisObject
     *
     * Removes this PaDvaMessage from the database. 
     * The calling application MUST then delete this PaDvaMessage object, as it makes no sense
     * to keep it any longer.
     *
     * @exception DatabaseException A DatabaseException will be thrown if there is a 
     *            problem writing the data to the database.
     *
     * pre: Either - this PaDvaMessage was initially loaded from the database
     *      OR     - writePaDvaMessageData() has already been called
     *      This PaDvaMessage has not been deleted
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
     * @exception DataConfigurationException If the data contained in the PaDvaMessage object
     *            is not sufficent to create an entry in the database, a 
     *            DataConfigurationException will be thrown. This is thrown as an exception
     *            and not tested as an assert as it can be directly linked to GUI components
     *            and may be the result of a user action that occurs prior to filling in 
     *            all sections.
     *
     * pre: This PaDvaMessage has not been deleted
     */
    void applyChanges();
    

    /**
     * getKey
     *
     * Returns the key for this PaDvaMessage.
     *
     * @return The key for this PaDvaMessage as an unsigned long.
     *
     * pre: Either - this PaDvaMessage was initially loaded from the database
     *      OR     - writePaDvaMessageData() has already been called
     *      AND    - deleteThisObject() has NOT been called
     */
    virtual unsigned long getKey();
    virtual unsigned long getLocationKey();
    virtual unsigned long getShortcutNo();
    virtual unsigned long getDVAMsgKey();

    virtual void setLocationKey( const unsigned long ulLocationKey );
    virtual void setShortcutNo( const unsigned long ulShortcutNo );
    virtual void setDVAMsgKey( const unsigned long ulDVAMsgKey );
    
    
     /**
     * getName
     *
     * Returns the name of this item for the IItem interface.  For the PA DVA Message this is 
     * just the key in a string format e.g. "PA Dva Message 1".
     *
     * @return The key description for this item as a std::string
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
     * setName
     *
     * Not applicable for PA DVA Messages.  Will just assert.
     *
     * @param name The name to give this item.
     *
     */
    virtual void setName(const std::string& name);


    /**
     * getAllItemChanges
     *
     * This is called to retrieve all changes made to this zone. If the map returned is empty then
     * no changes have been made. This must be called before the changes are applied or it will be
     * cleared.
     *
     * @return ItemChanges - The map containing all display changes
     */
    virtual TA_Base_Core::ItemChanges getAllItemChanges()
    {
        return m_paDvaMessageChanges;
    }

   /**
     * getUniqueIdentifier
     *
     * This retrieves the unique identifier for this operator. We cannot use keys for identification
     * in the configuration editor because new operators do not have keys. Therefore we use this
     * method for identification
     *
     * @return unsigned long - The unique identifier for this operator
     */
    virtual unsigned long getUniqueIdentifier()
    {
        return m_uniqueIdentifier;
    };

    /**
     * hasChanged
     *
     * This should be called to determine if any part of the PaDvaMessage has been changed by the user.
     *
     * @return bool - This will return true if the PaDvaMessage does not match the one in the database. It 
     *                will return false if the data has not changed from that in the database.
     */
    virtual bool hasChanged()
    {
        // If the PaDvaMessage changes is empty then nothing has changed
        return !m_paDvaMessageChanges.empty();
    }


    /**
     * isNew
     *
     * This should be called to determine if this PaDvaMessage is new. A new PaDvaMessage is one that has not
     * yet been applied to the database and been given a pkey and create date.
     *
     * @return bool - This will return true if the PaDvaMessage has not been applied to the database.
     */
    virtual bool isNew();


    /**
     * invalidate
     *
     * Make the data contained by this PaDvaMessage as invalid. The next call to get...() 
     * following a call to invalidate() will cause all the data to be reloaded from
     * the database.
     *
     * pre: deleteThisObject() has NOT been called
     */
    void invalidate();


    private:
	/**
     * updatePaBroadcastShortcutChanges
     *
     * This updates the map recording the changes to the PaDvaMessage. Whenever a call to set is made in this
     * class then this method must be called to store the changes.
     *
     * @param const string& - The name of the attribute that has been given a new value
     * @param const string& - The old value that was stored for this attribute
     * @param const string& - The new value to be stored for this attribute
     */
    virtual void updatePaBroadcastShortcutChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);

	/**
     * updatePaBroadcastShortcutChanges
     *
     * This updates the map recording the changes to the PaDvaMessage. Whenever a call to set is made in this
     * class then this method must be called to store the changes.  The input values are converted to
     * string representations before being added to the map
     *
     * @param const string& - The name of the attribute that has been given a new value
     * @param const unsigned long - The old value that was stored for this attribute
     * @param const unsigned long - The new value to be stored for this attribute
     */
    virtual void updatePaBroadcastShortcutChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue);

    // Assignment PaDvaMessage not used so it is made private
	ConfigPaBroadcastShortcut& operator=(const ConfigPaBroadcastShortcut&);

    unsigned long m_uniqueIdentifier; // We cannot use keys for identification of PaDvaMessages in the
                                      // Configuration Editor because new PaDvaMessages do not have keys.
                                      // Therefore we use this method for identification.

    static long s_nextAvailableIdentifier; // The next unique identifier available

    PaBroadcastShortcutHelper* m_pPaBroadcastShortcutHelper;

    TA_Base_Core::ItemChanges m_paDvaMessageChanges; // This is a map of all changes made to the PaDvaMessage so that they are
                                   // recorded and can be sent in audit or online update messages.

};

} // closes TA_IRS_Core

#endif // !defined(ConfigPaBroadcastShortcut_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
