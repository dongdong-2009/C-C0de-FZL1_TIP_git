/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/trends/src/TrendHelper.h $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This is the base TrendHelper class that supports all get and set operations on trends
  *
  * NOTE: This will also directly read the TREND_TEMPLATE table to decide if the trends requires a display
  */

#if !defined(TREND_HELPER_H)
#define TREND_HELPER_H

#include <string>
#include <map>

#include "core/data_access_interface/trends/src/ItemInfoDetails.h"

namespace TA_Base_Core
{
    class TrendHelper
    {

    public:
        /**
         * Constructor
         * 
         * This constructor creates a new TrendHelper for the specified name.
         *
         * @param name The key into the TREND_TEMPLATE table 
         */
        TrendHelper(const std::string& name);

        /**
         * Constructor
         * 
         * This constructor creates a new TrendHelper for configuration, initiailising
         * the key to 0 - it will be specified once this Trend is written to the database
         * for the first time.
         */
        TrendHelper();


        /**
         * Copy Constructor
         */
        TrendHelper( const TrendHelper& theHelper);


        /**
         * Destructor
         */
        virtual ~TrendHelper();


        /**
         * isNew
         *
         * This returns whether this is a new trend or not
         *
         * @return bool - True if this is a new trend
         */
        virtual bool isNew() const
        {
            return m_isNew;
        }

        /**
         * getName
         *
         * Returns the name for this Trend.
         *
         * @return The name for this Trend as an std::string.
         *
         * pre: Either - this gui was initially loaded from the database
         *      OR     - writeGuiData() has already been called
         */
        std::string getName();

		/**
         * setName
         *
         * Sets the name of this trend. This should only be used by the ConfigTrend class
         *
         * @param name The name to give this trend.
         *
         * pre: deleteThisObject() has NOT been called
         */
		void setName(const std::string& name);

        void setDescription(const std::string& description);


		/**
         * getGraphTypeAsString
         *
         * This method retrieves an std::string indicating the graph type
         *
         * @return An std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        std::string getGraphTypeAsString();

        
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
        time_t getDateCreated();


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
        time_t getDateModified();

        
        /**
         * invalidate
         *
         * Make the data contained by this profile as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         * It is a precondition that writeProfileData() has been called prior to calling
         * this method on a new gui, as it does not make any sense to invalidate an
         * gui that has not yet been written to the database.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - writeGuiData() has already been called
         */
        void invalidate();


        /**
         * writeData
         * 
         * Write this trend to the database. If this is a new trend, this will populate
         * the m_name field (which can be retrieved using getName()).
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Trend object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         */
        void writeData();


        /**
         * deleteTrend
         *
         * Remove this trend from the database. Once this method has been executed, the
         * TrendHelper object should be destroyed.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this trend was initially loaded from the database
         *      OR     - writeProfileData() has already been called
         */
        void deleteTrend();

        /**
         * getDescription
         *
         * Get the description.
         *
         * return std::string - description string.
         */
        std::string getDescription();

        /**
         * getRangeSeconds
         *
         * Get the the time range in seconds.
         *
         * return long - range in seconds.
         */
        long getRangeSeconds();
	    
        /**
         * getSampleTypeInSeconds
         *
         * Get the sample type.
         *
         * @return long - sample type in seconds.
         */
        long getSampleTypeInSeconds();
	    
        /**
         * getStartDateTime
         *
         * Get the start/date time
         *
         * @return time_t - start/date time as a time_t
         */
        time_t getStartDateTime();
	    
        /**
         * isLive
         *
         * Get the live mode.
         *
         * @return bool - True: is live, False: not live
         */
        bool isLive();
	    
        /**
         * getOffsetInSeconds
         *
         * Get the offset time.
         *
         * @return long - offset in seconds
         */
        long getOffsetInSeconds();
	    
        /**
         * getItems
         *
         * Get the items set for trending.
         *
         * @return std::map<std::string, ItemInfo> - item name, item index and item colour.
         */
        std::map<std::string, ItemInfo> getItems();

        /**
         * getExtraInfo
         *
         * Get the extra information.
         *
         * @return std::string - extra info string.
         */
        std::string getExtraInfo();

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
        void decomposeTimeComponent(long rangeTimeInSeconds, long& hours, long& minutes);

        /**
         * getSamplePeriodAsString
         *
         * Get the name of the sample period
         *
         * @return std::string - name of sample period, eg. 1 MIN, 1 DAY.
         */
        std::string getSamplePeriodAsString();

        /**
         * setGraphType
         *
         * Sets the graph type.
         *
         * @param graphType - Graph type as a string (LINE, BAR, COMPARISON, XY)
         */
        void setGraphType(const std::string& graphType);
	    
        /**
         * setRangeSeconds
         *
         * Sets the time range
         *
         * @param rangeSeconds - time range in seconds
         */
        void setRangeSeconds(long rangeSeconds);
	    
        /**
         * setSampleType
         *
         * Sets the Sample Period
         *
         * @param sampleInSeconds - sample period in seconds
         */
        void setSamplePeriod(long sampleInSeconds);
	    
        /**
         * setStartDateTime
         *
         * Sets the start/date time.
         *
         * @param startDateTime - start/date time as a time_t
         */
        void setStartDateTime(time_t startDateTime);
	    
        /**
         * setLive
         *
         * Sets the live status.
         *
         * @param live - flag representing live mode. True: live, False: not live.
         */
        void setLive(bool live);
	    
        /**
         * setOffset
         *
         * Sets the offset time.
         *
         * @param offsetInSeconds - offset time in seconds
         */
        void setOffset(long offsetInSeconds);
	    
        /**
         * setItems
         *
         * Sets the items for trending.
         *
         * @param items - items for trending. ItemInfo contains the item index and colour.
         */
        void setItems(const std::map<std::string, ItemInfo>& items);

        /**
         * setExtraInfo
         *
         * Sets the extra info.
         *
         * @param info - extra information.
         */
        void setExtraInfo(const std::string& info);

        /**
         * createItemString
         *
         * Create an item string from the map of items. The string will be in the following format:
         * <item name>\<colour>\<index>,<item name>\<colour>\<index>, ...
         *
         * @param items - map of items
         *
         * @return std::string - item string
         */
        static std::string createItemString(const std::map<std::string, ItemInfo>& items);
            
    private:

        // Made private as it is not needed
		TrendHelper& operator=(const TrendHelper &);

        void reload();

        void modifyExistingTrend();

        void addNewTrend();

        std::string retrieveDeletedField(const std::string& currentField, const std::string& columnName);

        void parseItems(const std::string& itemsString, std::map<std::string, ItemInfo>& items);

        void parseTimeRange(unsigned long timeDate, int& days, time_t& time);

        long getTotalTimeRangeInSeconds();

        std::string convertGraphTypeToString(int index);

        int convertGraphTypeStringToNumber(const std::string& graphTypeString);

    private:

        static const std::string DELETED_FLAG;
        static const std::string DEFAULT_GRAPH_TYPE;
        static const long DEFAULT_TIME_COMPONENT;
        
        bool m_isValidData;
		bool m_isNew;

        std::string m_name;
        std::string m_description;
        std::string m_graphType;
        long m_rangeSeconds;
        long m_samplePeriod;
        time_t m_startDateTime;
        bool m_isLive;
        long m_offsetTime;
        std::map<std::string, ItemInfo> m_items;
        std::string m_extraInfo;
        time_t m_dateCreated;
        time_t m_dateModified;

        std::string m_originalName;
    };

} // closes TA_Base_Core

#endif // !defined(PROFILE_HELPER_H)
