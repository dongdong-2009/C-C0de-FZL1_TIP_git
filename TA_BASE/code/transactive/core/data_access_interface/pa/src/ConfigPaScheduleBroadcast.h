/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_T00010200/3002/transactive/core/data_access_interface/pa/src/ConfigPaScheduleBroadcast.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2010/01/11 18:41:48 $
  * Last modified by: $Author: builder $
  * 
  * ConfigPaScheduleBroadcast is a concrete implementation of IConfigPaScheduleBroadcast, which is in turn an implementation
  * of IPaDvaMessage. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaDvaMessages.
  */


#ifndef CONFIGPASCHEDULEBROADCAST__INCLUDED_
#define CONFIGPASCHEDULEBROADCAST__INCLUDED_

#include <string>
#include <set>

#include "core/data_access_interface/pa/src/IConfigPaScheduleBroadcast.h"


namespace TA_Base_Core
{

class IData;
class PaScheduleBroadcastHelper;

class ConfigPaScheduleBroadcast : public IConfigPaScheduleBroadcast
{

public:
    /**
     * ConfigPaScheduleBroadcast (constructor)
     *
     * Constructs a new instance of the ConfigPaScheduleBroadcast with no id. This is used when
     * creating a *new* PaDvaMessage - that is, one that does not yet exist in the database.
     */
    ConfigPaScheduleBroadcast();

    /**
     * ConfigPaScheduleBroadcast (constructor)
     *
     * Constructs a new instance using the information provided in IData interface
     *
     * @see PaDvaMessageHelper::PaDvaMessageHelper(IData*)
     *
     */
    ConfigPaScheduleBroadcast(unsigned long row, TA_Base_Core::IData& data);

    /**
     * ConfigPaScheduleBroadcast (constructor)
     *
     * Constructs a new instance of ConfigPaScheduleBroadcast with the specified id. This is used 
     * when creating a ConfigPaScheduleBroadcast based around an existing PaDvaMessage in the database.
     *
     * @param key The key to this PaDvaMessage in the database.
     */
    ConfigPaScheduleBroadcast(const unsigned long idKey);


    /**
     * ConfigPaScheduleBroadcast (copy constructor)
     *
     * @param ConfigPaScheduleBroadcast& - The PaDvaMessage to make a copy of.
     */
    ConfigPaScheduleBroadcast( const ConfigPaScheduleBroadcast& theConfigPaScheduleBroadcast);

    
    virtual ~ConfigPaScheduleBroadcast();


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
    virtual void deleteThisObject();


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
    virtual void applyChanges();
    

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
    virtual unsigned long getDvaMsgKey();
    virtual unsigned long getCyclicTimes();
    virtual unsigned long getPriorityGroup();
    virtual time_t getStartTime();
    virtual time_t getEndTime();
    virtual std::string getScheduleName();
    virtual std::string getIntervalType();
    virtual std::string getSourceType();
    virtual std::string getStatus();
    virtual bool getIsEnable();
    virtual bool getAutoDel();
    virtual std::vector<unsigned long> getZoneKeys();

    virtual void setLocationKey( unsigned long ulLocationKey );
    virtual void setDvaMsgKey( unsigned long ulDvaMsgKey );
    virtual void setCyclicTimes( unsigned long ulCyclicTimes );
    virtual void setPriorityGroup( unsigned long ulPriorityGroup );
    virtual void setStartTime( time_t tmStartTime );
    virtual void setEndTime( time_t tmEndTime );
    virtual void setScheduleName( const std::string& strScheduleName );
    virtual void setIntervalType( const std::string& strIntervalType );
    virtual void setSourceType( const std::string& strSourceType );
    virtual void setStatus( const std::string& strStatus );
    virtual void setIsEnable( bool bEnable );
    virtual void setAutoDel( bool bAutoDel );
    virtual void setZoneKeys( const std::vector<unsigned long>& vecZoneKeys );

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
    // Assignment PaDvaMessage not used so it is made private
	ConfigPaScheduleBroadcast& operator=(const ConfigPaScheduleBroadcast&);

    PaScheduleBroadcastHelper* m_pPaScheduleBroadcastHelper;
};

} // closes TA_IRS_Core

#endif // !defined(CONFIGPASCHEDULEBROADCAST__INCLUDED_)
