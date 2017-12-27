/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/trends/src/ConfigTrend.h $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigTrend is an implementation of IConfigTrend. It holds the data specific to a Trend entry
  * in the database, and allows read-write access to that data.
  */

#if !defined(ConfigTrend_799425B5_5FB2_49ca_851D_421EE5042C78__INCLUDED_)
#define ConfigTrend_799425B5_5FB2_49ca_851D_421EE5042C78__INCLUDED_

#include <string>
#include <map>

#include "core/data_access_interface/trends/src/ITrend.h"
#include "core/data_access_interface/trends/src/IConfigTrend.h"
#include "core/data_access_interface/trends/src/ItemInfoDetails.h"

namespace TA_Base_Core
{

    class TrendHelper;

    class ConfigTrend : public IConfigTrend 
    {

    public:
	    
        /**
         * Constructor
         *
         * @param trendName - Name of trend
         */
        ConfigTrend(const std::string& trendName);

        /**
         * Constructor
         *
         */
        ConfigTrend();

        /**
         * ConfigProfile (copy constructor)
         *
         * @param ConfigProfile& - The profile to make a copy of.
         */
        ConfigTrend( const ConfigTrend& theTrendProfile);

        /**
         * Destructor
         *
         */
        virtual ~ConfigTrend() {}

        /**
         * setGraphType
         *
         * Sets the graph type.
         *
         * @param graphType - Graph type as a string (LINE, BAR, COMPARISON, XY)
         */
        virtual void setGraphType(const std::string& graphType);
	    
        /**
         * setRangeSeconds
         *
         * Sets the range (in seconds) of the time display of the graph
         *
         * @param rangeSeconds - range in seconds
         */
        virtual void setRangeSeconds(long rangeSeconds);
	    
        /**
         * setSampleType
         *
         * Sets the Sample Period
         *
         * @param sampleInSeconds - sample period in seconds
         */
        virtual void setSampleType(long sampleInSeconds);
	    
        /**
         * setStartDateTime
         *
         * Sets the start/date time.
         *
         * @param startDateTime - start/date time as a time_t
         */
        virtual void setStartDateTime(time_t startDateTime);
	    
        /**
         * setLive
         *
         * Sets the live status.
         *
         * @param live - flag representing live mode. True: live, False: not live.
         */
        virtual void setLive(bool live);
	    
        /**
         * setOffset
         *
         * Sets the offset time.
         *
         * @param offsetInSeconds - offset time in seconds
         */
        virtual void setOffset(long offsetInSeconds);
	    
        /**
         * setItems
         *
         * Sets the items for trending.
         *
         * @param items - items for trending. ItemInfo contains the item index and colour.
         */
        virtual void setItems(const std::map<std::string, ItemInfo>& items);

        /**
         * setExtraInfo
         *
         * Sets the extra info.
         *
         * @param info - extra information.
         */
        virtual void setExtraInfo(const std::string& info);

        /**
         * setDescription
         *
         * Sets the description.
         *
         * @param description - description as a string.
         */
        virtual void setDescription(const std::string& description);
	    
        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all item changes
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
        virtual bool hasChanged();
	    
        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew();
	    
        /**
         * setName
         *
         * Sets the name of this item locally.
         *
         * @param name The name to give this item.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the item key is invalid (and this is not a new item).
         */
        virtual void setName(const std::string& name);
	    
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
         * getDescription
         *
         * Get the description.
         *
         * return std::string - description string.
         */
        virtual std::string getDescription();
        
        /**
         * getGraphTypeAsString
         *
         * Get the graph type as a string.
         *
         * @return std::string - graph type as a string (LINE, BAR, COMPARION, XY)
         */
        virtual std::string getGraphTypeAsString();
           
        /**
         * getRangeSeconds
         *
         * Get the range (in seconds) of the time display of the graph
         *
         * @return long - range in seconds
         */
        virtual long getRangeSeconds();
        
        /**
         * getSampleTypeInSeconds
         *
         * Get the sample type.
         *
         * @return long - sample type in seconds.
         */
        virtual long getSampleTypeInSeconds();

        /**
         * getStartDateTime
         *
         * Get the start/date time
         *
         * @return time_t - start/date time as a time_t
         */
        virtual time_t getStartDateTime();
        
        /**
         * isLive
         *
         * Get the live mode.
         *
         * @return bool - True: is live, False: not live
         */
        virtual bool isLive();
        
        /**
         * getOffsetInSeconds
         *
         * Get the offset time.
         *
         * @return long - offset in seconds
         */
        virtual long getOffsetInSeconds();
        
        /**
         * getItems
         *
         * Get the items set for trending.
         *
         * @return std::map<std::string, ItemInfo> - item name, item index and item colour.
         */
        virtual std::map<std::string, ItemInfo> getItems();
       
        /**
         * getExtraInfo
         *
         * Get the extra information.
         *
         * @return std::string - extra info string.
         */
        virtual std::string getExtraInfo();

        /**
         * decomposeTimeComponent
         *
         * Decompose the time component of the range into hours and minutes.
         *
         * @param rangeTimeInSeconds - time component in seconds
         * @param hours - returned hours
         * @param minutes - return minutes
         *
         */
        virtual void decomposeTimeComponent(long rangeTimeInSeconds, long& hours, long& minutes);

        /**
         * getSamplePeriodAsString
         *
         * Get the name of the sample period
         *
         * @return std::string - name of sample period, eg. 1 MIN, 1 DAY.
         */
        virtual std::string getSamplePeriodAsString();
        
        virtual unsigned long getUniqueIdentifier()
        {
            return m_uniqueIdentifier;
        }
	    
        /**
         * getKey
         *
         * Returns the key for this profile.
         *
         * @return The key for this profile as an unsigned long.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - writeOperatorData() has already been called
         *      AND    - deleteThisObject() has NOT been called
         */
        unsigned long getKey();

        /**
         * getName
         *
         * Returns the name of this profile. If this is the first time data for this profile
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this profile as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - setName() has already been called
         *      AND    - deleteThisObject() has NOT been called
         */
        virtual std::string getName();

        /**
         * invalidate
         *
         * Make the data contained by this profile as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        virtual void invalidate();

        /**
         * deleteThisObject
         *
         * Removes this profile from the database. 
         * The calling application MUST then delete this profile object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This profile has not been deleted
         */
        virtual void deleteThisObject();

        /**
         * getDateCreated
         *
         * Get the date created
         *
         * @return time_t - date created
         */
        virtual time_t getDateCreated();

        /**
         * getDateModified
         *
         * Get the date modified
         *
         * @return time_t - date modified
         */
        virtual time_t getDateModified();
        
    private:

        ConfigTrend& operator=(const ConfigTrend &);
	
        void updateChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
        
        std::string convertTimeToString(time_t timeToConvert);

    private:
        
        TrendHelper* m_trendHelper;      
        
        ItemChanges m_trendChanges;                 // This is a map of all changes made to the profile so that they are
                                                    // recorded and can be sent in audit or online update messages.

	    static long  s_nextAvailableIdentifier;     // The next unique identifier available
        
	    unsigned long m_uniqueIdentifier;           // We cannot use keys for identification of trends in the
                                                    // Configuration Editor because new profiles do not have keys.
                                                    // Therefore we use this method for identification.

    };
}

#endif // !defined(ConfigTrend_799425B5_5FB2_49ca_851D_421EE5042C78__INCLUDED_)
